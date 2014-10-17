#version 120

// applies the Uncharted 2 filmic tonemapping formula to the framebuffer
// to produce RGB values in the range [0, 1] for display into the
// backbuffer - see http://filmicgames.com/archives/75

uniform sampler2D render;
uniform float exposure;

varying vec2 uv;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;

float white_point = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

void main()
{
    vec3 sample = max(texture2D(render, uv).rgb, vec3(0.0));

    // assume values > white_point map to white

    vec3 color = Uncharted2Tonemap(exposure * sample)
               / Uncharted2Tonemap(vec3(white_point));

    gl_FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
