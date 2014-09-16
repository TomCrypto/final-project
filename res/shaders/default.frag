#version 130

in vec2 uv;
uniform sampler2D tex;

void main()
{
    gl_FragColor = texture2D(tex, uv) * vec4(0.75, 0.25, 0.25, 1);
}