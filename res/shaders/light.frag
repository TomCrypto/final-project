#version 120

// simply draws a sphere with the given light color

uniform vec3 light_color;

void main()
{
    gl_FragColor = vec4(light_color, 1.0);
}
