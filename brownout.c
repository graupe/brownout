/* public domain */
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>
#include <limits.h>

#include "config.h"

char * argv0;

int
get_brown_percent(Display *dpy, Window root, Atom brown_atom) {
	int format;
	unsigned long n = 0, extra = 0;
	unsigned int *p = NULL;
	Atom atom;
	int result = 100;

	if(XGetWindowProperty(dpy, root, brown_atom, 0L, 1L, False, XA_CARDINAL,
			&atom, &format, &n, &extra, (unsigned char **)&p) == Success && n > 0) {
		 result = *p;
		 XFree(p);
	}
	return result;
}

void
set_brown_percent(Display *dpy, Window root, Atom brown_atom, int percent) {
	XChangeProperty(dpy, root, brown_atom, XA_CARDINAL, 32,
			PropModeReplace, (unsigned char*)&percent, 1);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [{+|-|=}PERCENT] increase,decrease or set the brownout by/to PERCENT\n", argv0);
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	Display *dpy = XOpenDisplay(NULL);
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);
	Atom brown_atom = XInternAtom(dpy, "_BROWN_PERCENT", False);
	XRRScreenResources *res = XRRGetScreenResourcesCurrent(dpy, root);
	int percent = get_brown_percent(dpy, root, brown_atom);
	argv0 = argv[0];


	if (argc <= 1)
		usage(); /* exits */

	 switch(argv[1][0]) {
		 case '+': percent+= atoi(argv[1]+1);
			   break;
		 case '-': percent-= atoi(argv[1]+1);
			   break;
		 default:  percent = atoi(argv[1]);
			   break;
	} 

	 if (percent > 100) percent = 100;
	 else if (percent < 0) percent = 0;

	 printf("%d\n", percent);

	 set_brown_percent(dpy, root, brown_atom, percent);

	 float gr = gammar(percent);
	 float gg = gammag(percent);
	 float gb = gammab(percent);

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
