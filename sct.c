#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xrender.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <math.h>


/* cribbed from redshift, but truncated with 500K steps */
static const struct { float r; float g; float b; } whitepoints[] = {
	{ 1.00,  0.58,  0.00, }, /* 1000K */
	{ 1.00,  0.67,  0.00, },
	{ 1.00,  0.75,  0.08, },
	{ 1.00,  0.80,  0.28, },
	{ 1.00,  0.84,  0.42, },
	{ 1.00,  0.88,  0.54, },
	{ 1.00,  0.90,  0.64, },
	{ 1.00,  0.92,  0.73, },
	{ 1.00,  0.95,  0.81, },
	{ 1.00,  0.97,  0.88, },
	{ 1.00,  0.98,  0.94, },
	{ 1.00,  1.00,  1.00, }, /* 6500K */
	{ 0.95,  0.96,  1.00, },
	{ 0.91,  0.94,  1.00, },
	{ 0.87,  0.92,  1.00, },
	{ 0.85,  0.90,  1.00, },
	{ 0.82,  0.89,  1.00, },
	{ 0.80,  0.87,  1.00, },
	{ 0.78,  0.86,  1.00, }, /* 10000K */
	{ 0.77,  0.85,  1.00, },
};

int
main(int argc, char **argv)
{
	Display *dpy = XOpenDisplay(NULL);
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);

	XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

	int temp = 6500;
	if (argc > 1)
		temp = atoi(argv[1]);
	if (temp < 1000 || temp > 10000)
		temp = 6500;

	temp -= 1000;
	double ratio = temp % 500 / 500.0;
#define AVG(c) whitepoints[temp / 500].c * (1 - ratio) + whitepoints[temp / 500 + 1].c * ratio
	double gammar = AVG(r);
	double gammag = AVG(g);
	double gammab = AVG(b);

	int num_crtcs = res->ncrtc;
	for (int c = 0; c < res->ncrtc; c++) {
		int crtcxid = res->crtcs[c];
		XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy, res, crtcxid);

		int size = XRRGetCrtcGammaSize(dpy, crtcxid);

		XRRCrtcGamma *crtc_gamma = XRRAllocGamma(size);

		for (int i = 0; i < size; i++) {
			double g = 65535.0 * i / size;
			crtc_gamma->red[i] = g * gammar;
			crtc_gamma->green[i] = g * gammag;
			crtc_gamma->blue[i] = g * gammab;
		}
		XRRSetCrtcGamma(dpy, crtcxid, crtc_gamma);

		XFree(crtc_gamma);
	}
}

