#version 120

//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

/* ============= END NOISE SHADER ============= */

struct light
{
    vec3 pos;
    vec3 strength;
};

uniform mat4 viewproj;
uniform vec3 view_dir;

uniform light lights[8];
uniform int light_count;

varying vec2 frag_pos;
varying vec2 uv;

void main()
{
    if (length(uv * 2 - vec2(1)) < 1) {
        vec3 radiance = vec3(0.0);
        
        for (int t = 0; t < light_count; ++t) {
            // amount of radiation falling on the film
            vec3 irradiance = lights[t].strength * pow(max(0.0, dot(normalize(lights[t].pos), view_dir)), 32);
            
            vec4 clip_space_pos = viewproj * vec4(lights[t].pos, 0.0f);
            vec2 screen_space_pos = clip_space_pos.xy / clip_space_pos.w;

            // weighted by the on-screen distance
            irradiance *= pow(max(0.0, 0.9 - min(length(screen_space_pos - frag_pos), 3.0) / 3.0), 14);
            
            radiance += irradiance;
        }
    
        // modulate incoming radiance with a little noise for diversity
        float noise = max(0, 0.88 + 0.24 * snoise(0.8 * (uv * 2 - vec2(1))));
        radiance *= vec3(noise);
        
        // and smooth out the edge of the film imperfection
        float edge_distance = abs(1.0 - length(uv * 2 - vec2(1)));
        if (edge_distance < 0.15) radiance *= edge_distance / 0.15;
        
        gl_FragColor = vec4(vec3(radiance), 1);
    }
    else
        gl_FragColor = vec4(0, 0, 0, 1);
}
