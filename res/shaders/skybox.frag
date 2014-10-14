#version 120

varying vec3 extinction;
varying vec3 inscattering;
varying vec3 eye_dir;

void main()
{
    if (eye_dir.y < 0) {
        gl_FragColor = vec4(0, 0.15, 0.85, 1);
    } else {
        gl_FragColor = vec4(inscattering / vec3(0.0138, 0.0113, 0.008), 1.0);
    }
}
