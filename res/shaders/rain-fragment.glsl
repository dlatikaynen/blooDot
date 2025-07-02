#version 330

/* a beautiful water ripples effect.
 * as currently written, 
 * it has been cobbled together from various open source
 * and public domain licensed bits off shadertoys scripts,
 * iq bits and stackoverflow answers
 *
 * https://www.shadertoy.com/view/4djSRW */

uniform float     iTime; // shader playback time (in seconds)
uniform sampler2D xtex0;

in vec2 fragCoord;
in vec4 v_color;
out vec4 fragColor;

#define MAX_RADIUS 2 // Maximum number of cells a ripple can cross.
#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)

float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
	p3 += dot(p3, p3.yzx + 19.19);
	
	return fract((p3.x + p3.y) * p3.z);
}

vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
	p3 += dot(p3, p3.yzx+19.19);
	
	return fract((p3.xx+p3.yz)*p3.zy);
}

void main(void)
{
	float resolution = 1;
	vec2 uv = fragCoord.xy; // / 768 * resolution;
	vec2 p0 = floor(uv);

	vec2 circles = vec2(0.);
	for (int j = -MAX_RADIUS; j <= MAX_RADIUS; ++j)
	{
		for (int i = -MAX_RADIUS; i <= MAX_RADIUS; ++i)
		{
			vec2 pi = p0 + vec2(i, j);
			vec2 hsh = hash22(pi); // just pi is faster but has artifacts
			vec2 p = pi + hash22(hsh);
			float t = fract(0.3*iTime + hash12(hsh));
			vec2 v = p - uv;
			float d = length(v) - (float(MAX_RADIUS) + 1.)*t;
			float h = 1e-3;
			float d1 = d - h;
			float d2 = d + h;
			float p1 = sin(31.*d1) * smoothstep(-0.6, -0.3, d1) * smoothstep(0., -0.3, d1);
			float p2 = sin(31.*d2) * smoothstep(-0.6, -0.3, d2) * smoothstep(0., -0.3, d2);

			circles += 0.5 * normalize(v) * ((p2 - p1) / (2. * h) * (1. - t) * (1. - t));
		}
	}

	circles /= float((MAX_RADIUS*2+1)*(MAX_RADIUS*2+1));

	float intensity = mix(0.01, 0.15, smoothstep(0.1, 0.6, abs(fract(0.05*iTime + 0.5)*2.-1.)));
	vec3 n = vec3(circles, sqrt(1. - dot(circles, circles)));
	vec3 color = texture(xtex0, uv/resolution - intensity*n.xy).rgb + 5.*pow(clamp(dot(n, normalize(vec3(1., 0.7, 0.5))), 0., 1.), 6.);

	fragColor = vec4(color.rgb, 0.68);
}
