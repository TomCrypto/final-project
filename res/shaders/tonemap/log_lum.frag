#version 120

uniform sampler2D render;

varying vec2 uv;

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main()
{
    vec3 color = texture2D(render, uv).rgb;

    gl_FragColor = vec4(color, log(luminance(color) + 1e-5));
}
