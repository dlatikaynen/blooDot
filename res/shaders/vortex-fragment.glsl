#version 330

/* showing a journey from one array to another
 * as it currently stands, this fragment shader 
 * has been stitched together from various open source
 * and public domain bits off shadertoy scripts,
 * and my original academic expertise about fractals
 *
 * https://www.shadertoy.com/view/MtX3DM
 * parts from kali-traps by bergi in 2015 under aGPL3 */

uniform float iTime;
uniform sampler2D xtex0;

in vec2 fragCoord;
in vec4 v_color;
out vec4 fragColor;

const int MAX_ITER = 69;
const vec3 KALI_COORD = vec3(.5, .4, 1.5);
const int KALI_MAXITER = 33;
const float Y_RESOLUTION = 768./1280.;

vec4 average;

// animation time
#define ti (iTime * 0.1 + 17.)

/* the kali fractal 
 * returns last magnitude,
 * and keeps track of the average */
float fract_kali3d(in vec3 p)
{
	average = vec4(0.);
	float mag;
	
	for (int i=0; i < KALI_MAXITER; ++i)
	{
		mag = dot(p, p);
		p = abs(p) / mag;
		average += vec4(p, mag);
		p -= KALI_COORD;
	}

	average /= 32.;

	return mag;
}

// steps from pos along dir, and samples the cloud
vec3 ray_color(vec3 pos, vec3 dir, float stp)
{
	vec3 p, col = vec3(0.);
	float t = 0.;

	for (int i = 0; i < MAX_ITER; ++i)
	{
		p = pos + t * dir;
		float d = fract_kali3d(p);

		// define a surface and capture
		d = 1. - abs(1. - d);
		
		// step within a certain range
		t += max(0.001, min(0.01, d )) * (stp + 3. * t);

		// (some color) over (distance to surface)
		col += (.8 - .3 * cos(average.rgb * 30.)) / (1. + d * d * 1000.);
	}
	
	return clamp(col / 5., 0., 1.);
}

void main(void)
{
	// ray step size (focus scale)
	float foc = 0.0024 + 0.06 * pow(sin(ti * 0.9) * .5 + .5, 2.);

	// positioning: a circular path depending on the step size
	float rti = ti * 0.2;
	float rad = foc;
	vec3 pos = (
				+ vec3(-2.3, 1.19, -3.4)
				+ (.001 + rad)
				* vec3(2. * sin(rti), cos(rti), 0.7 * sin(rti / 4.))
			   );
	
	vec2 uv = (fragCoord.xy / Y_RESOLUTION - .5) * 2.;
	vec3 dir = normalize(vec3(uv, 1.5 - length(uv))).xzy;
	
	dir.xz = vec2(sin(ti) * dir.x - cos(ti) * dir.z, cos(ti) * dir.x + sin(ti) * dir.z);

	vec3 col = ray_color(pos, dir, foc);

	fragColor = vec4(pow(col, vec3(1. / 1.3)), 1.);
}
