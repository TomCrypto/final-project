#version 120

varying vec3 inscattering;
varying vec3 eye_dir;
uniform float extinction;

void main()
{
    if (eye_dir.y < 0) {
        gl_FragColor = vec4(0, 0.15, 0.85, 1);
    } else {
        gl_FragColor = vec4(inscattering / extinction, 1.0);
    }
}
