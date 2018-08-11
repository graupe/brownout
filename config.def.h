/* public domain */
/* This resembles the original way of looking up values from a table.
 * Cribbed from redshift, but truncated with 500K steps and reduced accuracy to
 * one percent */
struct whitepoint { float r, g, b; };
static const struct whitepoint whitepoints[] = {
#if 0 // we don't want cooler colors
	{ 0.77,  0.85,  1.00, },
	{ 0.77,  0.85,  1.00, },
	{ 0.78,  0.86,  1.00, }, /* 10000K */
	{ 0.80,  0.87,  1.00, },
	{ 0.82,  0.89,  1.00, },
	{ 0.85,  0.90,  1.00, },
	{ 0.87,  0.92,  1.00, },
	{ 0.91,  0.94,  1.00, },
	{ 0.95,  0.96,  1.00, },
#endif
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
static float avg(int permille, int color)
{
	const int size = -1 + sizeof whitepoints/sizeof whitepoints[0];
	const int gapsize = 1000 / size;
	const int wpi = permille / gapsize;
	const float avgratio = (permille % gapsize) / (1.0*gapsize);
	const struct whitepoint * wp = &whitepoints[wpi];
	float c, cn;
	switch(color) {
		case red:   c = wp[0].r, cn = wp[1].r; break;
		case green: c = wp[0].g, cn = wp[1].g; break;
		default:    c = wp[0].b, cn = wp[1].b; break;
	}
	return c * (1.0 - avgratio) + cn * avgratio;
}
static float gammar(int permille) { return avg(permille, red); }
static float gammag(int permille) { return avg(permille, green); }
static float gammab(int permille) { return avg(permille, blue); }
