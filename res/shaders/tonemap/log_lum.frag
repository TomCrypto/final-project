#version 120

uniform sampler2D tex;

in vec2 uv;

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main()
{
    vec3 color = texture2D(tex, uv).rgb;

    gl_FragColor = vec4(color, log(luminance(color) + 1e-5f));
}
