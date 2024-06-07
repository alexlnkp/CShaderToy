#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LINK_SHADERS(...) link_shaders(__VA_ARGS__, 0)
#define REMOVE_SHADERS(...) remove_shaders(__VA_ARGS__, 0)

static GLuint global_shader_program = 0;

GLuint get_global_shader_program() {
    return global_shader_program;
}

void set_global_shader_program(GLuint shader_program) {
    global_shader_program = shader_program;
}

char* read_shader(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) return NULL;

    fseek(fp, 0, SEEK_END); long size = ftell(fp); fseek(fp, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    fread(buffer, sizeof(char), size, fp); fclose(fp);
    buffer[size] = '\0';

    return buffer;
}

GLuint compile_shader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint success;
    GLchar info_log[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Shader compilation failed: %s\n", info_log);
    }

    return shader;
}

GLuint link_shaders(GLuint first_shader, ...) {
    GLuint shader_program = glCreateProgram();
    va_list shaders;
    va_start(shaders, first_shader);
    
    GLuint shader = first_shader;
    do {
        glAttachShader(shader_program, shader);
        shader = va_arg(shaders, GLuint);
    } while (shader != 0);

    va_end(shaders);

    glLinkProgram(shader_program);
    GLint success;
    GLchar info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info_log);
        fprintf(stderr, "Shader program linking failed: %s\n", info_log);
    }
    return shader_program;
}

void remove_shaders(GLuint first_shader, ...) {
    va_list shaders;
    va_start(shaders, first_shader);
    
    GLuint shader = first_shader;
    do {
        glDeleteShader(shader);
        shader = va_arg(shaders, GLuint);
    } while (shader != 0);

    va_end(shaders);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        const char* vertex_shader_source = read_shader("vertex.frag");
        const char* fragment_shader_source = read_shader("colors.frag");

        GLuint vertex_shader = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
        GLuint fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

        glDeleteProgram(global_shader_program);
        global_shader_program = LINK_SHADERS(vertex_shader, fragment_shader);
        glUseProgram(global_shader_program);

        REMOVE_SHADERS(vertex_shader, fragment_shader);

        free(vertex_shader_source);
        free(fragment_shader_source);
    }
}

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(400, 400, "6624", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    const char* vertex_shader_source   = read_shader("vertex.frag");
    const char* fragment_shader_source = read_shader("colors.frag");

    GLuint vertex_shader   = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    free(vertex_shader_source);
    free(fragment_shader_source);

    // Link shaders and export the program
    global_shader_program = LINK_SHADERS(vertex_shader, fragment_shader);

    REMOVE_SHADERS(vertex_shader, fragment_shader);

    // create da triangle
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left  
        0.5f, -0.5f, 0.0f, // Right 
        0.0f,  0.5f, 0.0f  // Top   
    }; 

    // Use our shader program when we want to render an object
    glUseProgram(global_shader_program);

    GLuint VBO, VAO;
    // Generate and bind a Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind a Vertex Buffer Object
    glGenBuffers(1, &VBO);
    // Bind our Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Load the vertex data into the Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    GLuint iTimeUniform = glGetUniformLocation(global_shader_program, "iTime"      );
    GLuint iReslUniform = glGetUniformLocation(global_shader_program, "iResolution");

    // The render loop
    while (!glfwWindowShouldClose(window)) {

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Update the 'time' uniform in the fragment shader
        float timeValue = glfwGetTime();
        glUniform1f(iTimeUniform, timeValue);

        glUniform2f(iReslUniform, 400.f, 400.f);

        // Draw the triangle
        glUseProgram(global_shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(global_shader_program);

    glfwTerminate();
    return 0;
}