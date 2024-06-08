#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LINK_SHADERS(...) link_shaders(__VA_ARGS__, 0)
#define REMOVE_SHADERS(...) remove_shaders(__VA_ARGS__, 0)

#define ARR_LEN(x) (sizeof(x) / sizeof(x[0]))

/// Initialize GLFW
#define _INIT_GLFW()                           \
    if (!glfwInit()) {                         \
        fprintf(stderr, "GLFW init failed\n"); \
        return -1;                             \
    }

/// Initialize GLEW
#define _INIT_GLEW()                           \
    if (glewInit() != GLEW_OK) {               \
        fprintf(stderr, "GLEW init failed\n"); \
        return -1;                             \
    }

/// Allocate `size` bytes and set `ptr` to point to it
/// Print an error message and exit if allocation fails
#define ALLOC(ptr, size)                       \
    *(ptr) = malloc(size);                     \
    if (*(ptr) == NULL) {                      \
        printf("Failed to allocate!\n");       \
        exit(1);                               \
    }

/// GL Check Program `success` value
#define GLCH_PROG_SUCCESS(success, info, prog, l_s, fmt) \
    if (!success) {                                      \
        glGetProgramInfoLog(prog, l_s, NULL, info);      \
        printf(fmt, info);                               \
    }

/// GL Check Shader `success` value
#define GLCH_SHADER_SUCCESS(success, info, shader, l_s, fmt) \
    if (!success) {                                          \
        glGetShaderInfoLog(shader, l_s, NULL, info);         \
        printf(fmt, info);                                   \
    }

#define WINDOW_WIDTH  600 // Default window width
#define WINDOW_HEIGHT 600 // Default window height

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

    char* buffer; ALLOC(&buffer, size + 1);

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
    GLCH_SHADER_SUCCESS(success, info_log, shader, 512, "Shader compilation failed: %s\n");

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
    GLCH_PROG_SUCCESS(success, info_log, shader_program, 512, "Shader program linking failed: %s\n");

    return shader_program;
}

/// @brief Removes a list of shaders
/// @param first_shader First shader to remove
/// @param ... The rest of the shaders to remove
/// @note The last shader must be 0
/// @note You may also use REMOVE_SHADERS() macro to not specify 0 in the end
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

void resize_callback(GLFWwindow* window, int width, int height) {
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

/// @brief Triangulates a convex polygon
/// @param polygonVertices Array of vertices
/// @param vertexCount Num of vertices
/// @param indices Array of indices
/// @param indexCount Num of indices
/// @note Assuming the polygon vertices are defined in a CCW order and are convex
void TriangulateConvexPolygon(const float* polygonVertices, size_t vertexCount, GLuint** indices, size_t* indexCount) {
    if (vertexCount < 3) {
        // Not enough vertices to form a polygon
        *indices = NULL;
        *indexCount = 0;
        return;
    }

    // The number of triangles is vertexCount - 2
    *indexCount = (vertexCount - 2) * 3;
    *indices = malloc(*indexCount * sizeof(GLuint));
    if (*indices == NULL) {
        *indexCount = 0;
        return;
    }

    // Assuming the polygon vertices are defined in a CCW order and are convex
    for (size_t i = 0; i < vertexCount - 2; i++) {
        (*indices)[3 * i + 0] = 0;          // Always the first vertex
        (*indices)[3 * i + 1] = i + 1;
        (*indices)[3 * i + 2] = i + 2;
    }
}

/// @brief Creates indices for a triangle strip
/// @details Agnostic of the polygon vertices order, instead links them together by their index in the array
/// @param vertexCount Num of vertices
/// @param indices Array of indices
/// @param indexCount Num of indices
/// @note The order must be Top -> Down -> Left -> Right
/// @example Pentagon:
///           0.0f,  1.0f, 0.0f, <- Top vertex
///          -1.0f,  0.3f, 0.0f, <- Middle left vertex
///           1.0f,  0.3f, 0.0f, <- Middle right vertex
///          -0.6f, -1.0f, 0.0f, <- Bottom left vertex
///           0.6f, -1.0f, 0.0f, <- Bottom right vertex
void CreateTriangleStripIndices(size_t vertexCount, GLuint** indices, size_t* indexCount) {
    if (vertexCount < 3) {
        // Not enough vertices to form a single triangle
        *indices = NULL;
        *indexCount = 0;
        return;
    }

    // The number of triangles is vertexCount - 2, each triangle has 3 indices
    *indexCount = (vertexCount - 2) * 3;
    *indices = malloc(*indexCount * sizeof(GLuint));

    // Check for successful allocation
    if (!*indices) {
        *indexCount = 0;
        return;
    }

    // Connect each set of three consecutive vertices
    for (size_t i = 0; i < vertexCount - 2; i++) {
        (*indices)[3 * i + 0] = i;
        (*indices)[3 * i + 1] = i + 1;
        (*indices)[3 * i + 2] = i + 2;
    }
}

int main(void) {
    _INIT_GLFW();

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
 
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_FALSE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "6624", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, resize_callback);

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    _INIT_GLEW();

    const char* vertex_shader_source   = read_shader("vertex.frag");
    const char* fragment_shader_source = read_shader("colors.frag");

    GLuint vertex_shader   = compile_shader(vertex_shader_source, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_shader_source, GL_FRAGMENT_SHADER);

    free(vertex_shader_source);
    free(fragment_shader_source);

    // Link shaders and export the program
    global_shader_program = LINK_SHADERS(vertex_shader, fragment_shader);

    REMOVE_SHADERS(vertex_shader, fragment_shader);

    // This is a square
    GLfloat vertices[] = {
        -1.0f, 1.0f, 0.0f,  // Top left
        1.0f, 1.0f, 0.0f,   // Top right
        -1.0f, -1.0f, 0.0f, // Bottom left
        1.0f, -1.0f, 0.0f,  // Bottom right
    };

    // This is da triangle
    // GLfloat vertices[] = {
    //      0.0f,  0.5f, 0.0f, // Top
    //     -0.5f, -0.5f, 0.0f, // Left
    //      0.5f, -0.5f, 0.0f, // Right
    // };

    GLuint* indices;
    size_t indexCount;
    size_t vertexCount = (size_t)(ARR_LEN(vertices) / 3);

    // TriangulateConvexPolygon(vertices, vertexCount, &indices, &indexCount);
    CreateTriangleStripIndices(vertexCount, &indices, &indexCount);

    // Use our shader program when we want to render an object
    glUseProgram(global_shader_program);

    GLuint VBO, VAO, EBO;
    // Generate and bind a Vertex Array Object
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Generate and bind a Vertex Buffer Object
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLuint*)0);

    glEnableVertexAttribArray(0);

    // Unbind the VBO (not the EBO) as it's saved in the VAO; the EBO will be unbound when the VAO is unbound.
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0);

    GLuint iTimeUniform = glGetUniformLocation(global_shader_program, "iTime"      );
    GLuint iReslUniform = glGetUniformLocation(global_shader_program, "iResolution");

    float timeValue;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // The render loop
    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &width, &height);
        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        // Update the 'time' uniform in the fragment shader
        timeValue = glfwGetTime();
        glUniform1f(iTimeUniform, timeValue);

        glUniform2f(iReslUniform, (float)width, (float)height);

        // Draw the triangle
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    free(indices);
    glDeleteProgram(global_shader_program);

    glfwTerminate();
    return 0;
}