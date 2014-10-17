#version 120

// see overlay.frag

varying vec2 frag_pos;
varying vec2 uv;

void main()
{
    uv = gl_MultiTexCoord0.xy;
    frag_pos = gl_Vertex.xy;
    gl_Position = gl_Vertex;
}
