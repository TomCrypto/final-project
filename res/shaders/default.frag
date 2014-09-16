#version 120

uniform sampler2D tex;
uniform float mip_level;
uniform float pixel_count;
uniform float exposure;

varying vec2 uv;

float luminance(vec3 color)
{
    return dot(color, vec3(0.2126f, 0.7152f, 0.0722f));
}

void main()
{
    float avg_log_lum = texture2D(tex, uv, mip_level).a;
    vec3 color = texture2D(tex, uv).rgb;

    float avg_lum = exp(avg_log_lum / pixel_count);
    float key = exposure / avg_lum;

    gl_FragColor = vec4(color.rgb * (key / (1.0f + luminance(color.rgb) * key)), 1);
}
