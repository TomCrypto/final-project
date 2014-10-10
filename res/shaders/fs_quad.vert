/* Generic vertex shader for fullscreen quads */

#version 120

attribute vec2 pos; // from (-1, -1) to (1, 1)
varying vec2 uv; // from (0, 0) to (1, 1)

void main(void)
{
    gl_Position = vec4(pos, 0.5, 1.0);
    uv = (gl_Position.xy + 1.0) / 2.0;
}
