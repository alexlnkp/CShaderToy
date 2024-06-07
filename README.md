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
      <p>GLSL shaders are widely-used by <b>MOST</b> game engines, including some of fan favorite, Godot and GameMaker, so learning shaders can come in really handy!</p>
    </td>
  </tr>
</table>

## Getting started
1. Install `glfw3` and `glew` packages using your package manager, or simply build and install them from source.
2. To build, do either one of these:
-   Use `./autobuild` shell script for simple building of the project. No no bloat, build systems, pure GCC
-   Use GCC itself, don't forget to provide needed flags for libraries that we're using here!
3. ???
4. Profit!

Since GLSL shaders can compile at run-time, You don't even have to recompile the main C source file every time you make a small change to the shader!
Just restart the compiled binary and it will automatically re-compile the shaders for You! 
This also means that if there's an error in the shader's code, it will be printed out to you verbosely and You will be able to figure out what went wrong!
All of that - on-the-go, no time wasted waiting for the whole program to compile and link.

In future a hotkey for reloading and recompiling shaders will be added, so stay tuned for that!

Here's a small demonstration video

![demo](docs/demo.mp4)


## What are you watining for? Go ahead and make something cool!