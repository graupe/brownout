/* public domain */
#if 1
/* use math to calculate new color values. The default config goes from normal
 * to brain malfunction yellow */
#include <math.h>
/* each functions output is used to scale the relevant color channel */
static float gammar(int percent) { return (.9 + .1*(1-.01*percent)); }
static float gammag(int percent) { return (.3 + .1*(1-.01*percent) + .6*exp((-.01*percent)/3.0)); }
static float gammab(int percent) { return (.0 + .1*(1-.01*percent) + .9*exp( -.01*percent)); }

#else
/* This resembles the original way of looking up values from a table.
 * Cribbed from redshift, but truncated with 500K steps and reduced accuracy to
 * one percent */
struct whitepoint { float r, g, b; };
static const struct whitepoint whitepoints[] = {
	{ 0.77,  0.85,  1.00, },
	{ 0.77,  0.85,  1.00, },
	{ 0.78,  0.86,  1.00, }, /* 10000K */
	{ 0.80,  0.87,  1.00, },
	{ 0.82,  0.89,  1.00, },
	{ 0.85,  0.90,  1.00, },
	{ 0.87,  0.92,  1.00, },
	{ 0.91,  0.94,  1.00, },
	{ 0.95,  0.96,  1.00, },
	{ 1.00,  1.00,  1.00, }, /* 6500K */
	{ 1.00,  0.97,  0.94, },
	{ 1.00,  0.93,  0.88, },
	{ 1.00,  0.90,  0.81, },
	{ 1.00,  0.86,  0.73, },
	{ 1.00,  0.82,  0.64, },
	{ 1.00,  0.77,  0.54, },
	{ 1.00,  0.71,  0.42, },
	{ 1.00,  0.64,  0.28, },
	{ 1.00,  0.54,  0.08, },
	{ 1.00,  0.42,  0.00, },
	{ 1.00,  0.18,  0.00, }, /* 1000K */
};
enum { red, green, blue};
static float avg(int percent, int color)
{
	const int size = -1 + sizeof whitepoints/sizeof whitepoints[0];
	const int gapsize = 100 / size;
	const int wpi = percent / gapsize;
	const float avgratio = (percent % gapsize) / (1.0*gapsize);
	const struct whitepoint * wp = &whitepoints[wpi];
	float c, cn;
	switch(color) {
		case red:   c = wp[0].r, cn = wp[1].r; break;
		case green: c = wp[0].g, cn = wp[1].g; break;
		default:    c = wp[0].b, cn = wp[1].b; break;
	}
	return c * (1.0 - avgratio) + cn * avgratio;
}
static float gammar(int percent) { return avg(percent, red); }
static float gammag(int percent) { return avg(percent, green); }
static float gammab(int percent) { return avg(percent, blue); }
#endif
