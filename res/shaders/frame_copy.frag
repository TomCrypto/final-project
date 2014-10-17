#version 120

// simple framebuffer copy, direct copy of the input texture to render target

uniform sampler2D render;

varying vec2 uv;

void main()
{
	gl_FragColor = texture2D(render, uv);
}
