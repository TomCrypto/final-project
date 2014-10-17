#version 120

// light_f: index of the light the ghosts belong to
// ghost_f: index of the ghost for this light source

varying vec2 uv;
varying float light_f;
varying float ghost_f;

void main()
{
    light_f = gl_MultiTexCoord0.z;
    ghost_f = gl_MultiTexCoord0.w;
    uv = gl_MultiTexCoord0.xy;
    gl_Position = gl_Vertex;
}
