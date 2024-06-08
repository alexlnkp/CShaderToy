# CShaderToy

### It's that time of the year again, fellas!

<table>
  <tr>
    <td valign="top">
      <img src="https://github.com/alexlnkp/CShaderToy/assets/79400603/44c5f5ea-9e85-4b3a-907f-3912847484f3" alt="infamous-triangle" style="display: block; margin-left: auto; margin-right: auto;" />
    </td>
    <td valign="top">
      <p>Time to learn new stuff, and not just something that is "kinda" cool, but rather something that is <u>EXTREMELY</u> cool!</p>
      <p>For those who thought that programming and art could NOT be combined together, ShaderToy came in to prove You wrong!</p>
      <p>Even though Shader<i>Toy</i> literally has <i>"Toy"</i> in its name, reality is that GLSL shaders are not limited to just being a fun thing to play around with!</p>
      <p>This is when the idea to making this project came to my mind - ShaderToy, but local and on C</p>
    </td>
  </tr>
</table>

GLSL shaders are widely-used by **MOST** game engines, including some of fan favorites, Godot and GameMaker; in other words - learning shaders can come in really handy!

## Getting started
1. Install `glfw3` and `glew` packages using your package manager, or simply build and install them from source.
2. To build, do either one of these:
-   Use `./autobuild` shell script for simple building of the project. Zero bloat and build systems, just pure GCC
-   Use GCC itself, don't forget to provide needed flags for libraries that we're using here!
3. ???
4. Profit!

Since GLSL shaders can compile at run-time, You don't even have to recompile the main C source file every time you make a small change to the shader!
Just click `R` on your keyboard and all of the shaders will automatically be re-compiled for You! 
This also means that if there's an error in the shader's code, it will be printed out to you verbosely and You will be able to figure out what went wrong!
All of that - on-the-go, no time wasted waiting for the whole program to compile, link, or even restart!

Here's a small demonstration video

[![demo](https://i.ytimg.com/vi/j519qeBmu3Y/maxresdefault.jpg)](https://youtu.be/j519qeBmu3Y "CShaderToy demo")

## Examples
<table>
  
  <tr>
    <td valign="center" halign="center">
      <img src="https://github.com/alexlnkp/CShaderToy/assets/79400603/1fc9d4ef-473f-4439-908e-9a90b26b80e2" alt="infamous-triangle" width="330" style="display: block; margin-left: auto; margin-right: auto;" />
    </td>
  <td valign="top">
  
      #version 330 core
  
      uniform float iTime;
      uniform vec2 iResolution;
      out vec4 fragColor;
  
      void main() {
          vec2 uv = gl_FragCoord.xy / iResolution;
  
          vec3 x = iTime * 0.25 + uv.xyx + vec3(0, 2, 4);
  
          vec3 col = 0.5 + 0.5 * sin(cos(x * 0.5) + sin(x * x * 0.5));
  
          fragColor = vec4(col, 1.0);
      }
    
  </td>
  </tr>
  <tr>
    <td valign="center" halign="center">
      Made by <a href="https://www.kishimisu.art/">kishimisu</a>!
      <img src="https://github.com/alexlnkp/CShaderToy/assets/79400603/fa816392-e44d-4fea-9d3d-af0ebd25fefd" alt="kishimasu shader art" width="330" style="display: block; margin-left: auto; margin-right: auto;" />
    </td>
    <td valign="top">

    #version 330 core

    uniform float iTime;
    uniform vec2 iResolution;
    out vec4 fragColor;

    vec3 palette(float t) {
        return vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5) * cos(
            6.28318 * (vec3(1.0, 1.0, 1.0) * t + vec3(0.263, 0.416, 0.557))
        );
    }

    void main() {
        vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution) / iResolution.y;
        vec2 uv0 = uv;
        vec3 finalColor = vec3(0.0);

        for (float i = 0.0; i < 3.0; i++) {
            uv = fract(uv * 1.5) - 0.5;

            float d = length(uv) * exp(-length(uv0));
            d = pow(0.01 / abs(sin(d * 8.0 + iTime) / 8.0), 1.2);

            finalColor += palette(length(uv0) + i * 0.4 + iTime * 0.4) * d;
        }

        fragColor = vec4(finalColor, 1.0);
    }
  </td>
  </tr>
  <tr>
    <td valign="center" halign="center">
      <img src="https://github.com/alexlnkp/CShaderToy/assets/79400603/280e21d1-a0e7-46f4-879f-ce074c2b60d0" alt="my shader :)" width="330" style="display: block; margin-left: auto; margin-right: auto;" />
    </td>
    <td valign="top">

    #version 330 core

    uniform float iTime;
    uniform vec2 iResolution;
    out vec4 fragColor;
    
    vec3 palette(float t) {
        return vec3(0.610, 0.498, 0.650) + vec3(0.388, 0.498, 0.350) * cos(
            6.28314 * (vec3(0.530, 0.498, 0.620) *
            t + vec3(3.438, 3.012, 4.025)));
    }
    
    void main() {
        vec2 uv = gl_FragCoord.xy / iResolution;
        uv *= iResolution.x / iResolution.y;
    
        vec3 finalColor = vec3(0.0);
        float brightness = 0.2;
    
        float uvLength = length(uv);
        float timeOffset = iTime * 0.3;
    
        for (float i = 0.0; i < 1.0; i += 0.1) {
            float phase = (iTime + uv.y + (uv.x * pow(2.0, i * 1.9))) * 3.5;
            float shape = abs(0.8 * sin(phase));
    
            vec3 col = palette(uvLength + (i * 0.3) + timeOffset) * brightness;
    
            shape = pow(0.08 / shape, 1.3);
            finalColor += col * shape;
        }
    
        fragColor = vec4(finalColor, 1.0);
    }
  </td>
  </tr>
  
</table>

## What are you waiting for? Go ahead and make something cool!