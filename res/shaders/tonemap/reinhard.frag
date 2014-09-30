#version 120

uniform sampler2D render;
uniform float exposure;

varying vec2 uv;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec3 texColor = texture2D(render, uv).rgb;

    // assume values > W map to white

    vec3 curr = Uncharted2Tonemap(exposure * texColor);

    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    vec3 color = curr * whiteScale;

    gl_FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
}
