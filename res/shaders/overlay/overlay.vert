#version 120

varying vec2 frag_pos;
varying vec2 uv;

uniform float inv_ratio;

void main()
{
    gl_Position = gl_Vertex * vec4(inv_ratio, 1, 1, 1);
    frag_pos = gl_Vertex.xy;
    uv = gl_MultiTexCoord0.xy;
}
