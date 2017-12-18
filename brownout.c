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

#ifndef VERSION
#define VERSION "unknown"
#endif

Display *dpy;
Window root_window;
Atom brown_atom;

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
getbrownvalue(char *arg)
{
	int percent = atoi(arg);

	if (arg[0] == '+' || arg[0] == '-') {
		int format;
		unsigned long n = 0, extra = 0;
		unsigned int *p = NULL;
		Atom atom;
		if(XGetWindowProperty(dpy, root_window, brown_atom, 0L, 1L, False, XA_CARDINAL,
					&atom, &format, &n, &extra, (unsigned char **)&p) == Success && n > 0) {
			percent += *p;
			XFree(p);
		}
	}
	if (percent < 0) return 0;
	if (percent > 100) return 100;
	return percent;
}

static void
setbrownvalue(int value)
{
	XChangeProperty(dpy, root_window, brown_atom, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char*)&value, 1);
}

static void
brownoutscreen(int screen, float gr, float gg, float gb)
{
	XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, RootWindow(dpy, screen));

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
brownallscreens(int percent)
{
	int screen, nscreens = ScreenCount(dpy);
	float gr = gammar(percent);
	float gg = gammag(percent);
	float gb = gammab(percent);
	for (screen = 0; screen < nscreens; screen++) {
		brownoutscreen(screen, gr, gg, gb);
	}
}

int
main(int argc, char **argv)
{
	int percent = 0;

	if (argc < 2)
		die("usage: %s [+|-]PERCENT brownscreen to[by] PERCENT\n", argv[0]);

	if (!strcmp("-v", argv[1]))
		die("brownout-"VERSION", public domain software\n");

	if (!(dpy = XOpenDisplay(NULL)))
		die("Cannot open display\n");

	root_window = RootWindow(dpy, DefaultScreen(dpy));
	brown_atom = XInternAtom(dpy, "_BROWN_PERCENT", False);

	percent = getbrownvalue(argv[1]);
	brownallscreens(percent);
	setbrownvalue(percent);
	printf("%d\n", percent);

	XCloseDisplay(dpy);

	return EXIT_SUCCESS;
}
