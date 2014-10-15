#version 120

uniform sampler2D render;

varying vec2 uv;

void main()
{
	gl_FragColor = texture2D(render, uv);
}
