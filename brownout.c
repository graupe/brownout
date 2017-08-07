/* public domain */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>

#include "config.h"

char * argv0;

static void
die(const char *errstr, ...)
{
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

static int
getintatom(Display *dpy, Window root, Atom brown_atom)
{
	int format;
	unsigned long n = 0, extra = 0;
	unsigned int *p = NULL;
	Atom atom;
	int result = 0;

	if(XGetWindowProperty(dpy, root, brown_atom, 0L, 1L, False, XA_CARDINAL,
			&atom, &format, &n, &extra, (unsigned char **)&p) == Success && n > 0) {
		 result = *p;
		 XFree(p);
	}
	return result;
}

static void
setintatom(Display *dpy, Window root, Atom brown_atom, int value)
{
	XChangeProperty(dpy, root, brown_atom, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char*)&value, 1);
}

static void
brownoutscreen(Display *dpy, int screen, float gr, float gg, float gb)
{
	Window root = RootWindow(dpy, screen);
	XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);

	for (int c = 0; c < res->ncrtc; c++) {
		int crtcxid = res->crtcs[c];
		int size = XRRGetCrtcGammaSize(dpy, crtcxid);
		XRRCrtcGamma *crtc_gamma = XRRAllocGamma(size);

		for (int i = 0; i < size; i++) {
			float g = 1.0 * USHRT_MAX * i / (size-1);
			crtc_gamma->red[i] = g * gr;
			crtc_gamma->green[i] = g * gg;
			crtc_gamma->blue[i] = g * gb;
		}
		XRRSetCrtcGamma(dpy, crtcxid, crtc_gamma);

		XFree(crtc_gamma);
	}
}

static void
brownallscreens(Display *dpy, int percent)
{
	int screen, nscreens = ScreenCount(dpy);
	float gr = gammar(percent);
	float gg = gammag(percent);
	float gb = gammab(percent);
	for (screen = 0; screen < nscreens; screen++) {
		brownoutscreen(dpy, screen, gr, gg, gb);
	}
}

static int
setup(Display *dpy, int mode, int percent)
{
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);
	Atom brown = XInternAtom(dpy, "_BROWN_PERCENT", False);

	switch(mode) {
		case '-': percent = getintatom(dpy, root, brown) - percent; break;
		case '+': percent = getintatom(dpy, root, brown) + percent; break;
	}
	if (percent > 100) percent = 100;
	else if (percent < 0) percent = 0;

	setintatom(dpy, root, brown, percent);
	return percent;
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [+|-]PERCENT increase,decrease or set the brownout by/to PERCENT\n", argv0);
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	char mode = 0;
	int percent = 0;
	Display *dpy;
	argv0 = argv[0];

	if (argc < 2)
		usage(); /* exits */

	if (!strcmp("-v", argv[1]))
		die("brownout-"VERSION", public domain software\n");

	switch(argv[1][0]) {
		case '+':
		case '-': mode = argv[1][0];
			  argv[1]++;
	} 

	percent = atoi(argv[1]);

	if (!(dpy = XOpenDisplay(NULL)))
		die("Cannot open display\n");

	percent = setup(dpy, mode, percent);
	printf("%d\n", percent);
	brownallscreens(dpy, percent);

	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}
