#version 120

uniform vec3 sun_color;

void main()
{
    gl_FragColor = vec4(sun_color, 1.0);
}
