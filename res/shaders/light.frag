#version 120

uniform vec3 light_color;

void main()
{
    gl_FragColor = vec4(light_color, 1.0);
}
