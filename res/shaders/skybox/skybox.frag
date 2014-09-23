#version 120

varying vec3 pos;

void main()
{
    float phi = atan(pos.x, pos.z);
    float theta = acos(pos.y / length(pos));

    vec3 topColor = vec3(0, 0, 1);
    vec3 bottomColor = vec3(1, 1, 1);

    vec3 color = mix(topColor, bottomColor, pow(theta / 3.14159265 * 1.9, 5));

    gl_FragColor = vec4(color, 1);
}
