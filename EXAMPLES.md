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