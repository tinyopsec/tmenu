#define _POSIX_C_SOURCE 200809L
#define FONT     "-*-fixed-medium-r-*-*-13-*-*-*-*-*-*-*"
#define FGNORM   "#bbbbbb"
#define BGNORM   "#222222"
#define FGSEL    "#eeeeee"
#define BGSEL    "#005577"
#define TOP      1
#define LINES    0
#define PROMPT   NULL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAXITEMS 65536
#define MAXTEXT  256
#define BUFSZ    4096

static Display *dp;
static Window w, root;
static int scr, sh, wh, ww;
static GC gc;
static Pixmap pm;
static XFontStruct *fs;
static int fa, fh;
static char *its[MAXITEMS];
static int ni = 0;
static char *mt[MAXITEMS];
static int nm = 0, sel = 0, hoff = 0;
static char tb[MAXTEXT];
static int tl = 0;
static int top = TOP;
static int ln = LINES;
static const char *pr = PROMPT;
static unsigned long col[4];

static unsigned long xc(const char *s) {
	XColor c;
	Colormap cm = DefaultColormap(dp, scr);
	if(!XParseColor(dp, cm, s, &c) || !XAllocColor(dp, cm, &c)) exit(1);
	return c.pixel;
}

static void rd(void) {
	char buf[BUFSZ];
	while(fgets(buf, sizeof buf, stdin)) {
		int l = strlen(buf);
		if(l && buf[l-1] == '\n') buf[--l] = 0;
		if(l && buf[l-1] == '\r') buf[--l] = 0;
		if(!l) continue;
		if(!(its[ni] = strdup(buf))) exit(1);
		if(++ni >= MAXITEMS) break;
	}
}

static void lc(char *d, const char *s) { while((*d++ = tolower((unsigned char)*s++))); }

static void fi(void) {
	char lo[MAXTEXT], tmp[BUFSZ];
	int i;
	sel = hoff = nm = 0;
	lc(lo, tb);
	for(i = 0; i < ni; i++) {
		lc(tmp, its[i]);
		if(!tl || !strncmp(tmp, lo, tl)) mt[nm++] = its[i];
	}
	if(!tl) return;
	for(i = 0; i < ni; i++) {
		lc(tmp, its[i]);
		if(strncmp(tmp, lo, tl) && strstr(tmp, lo)) mt[nm++] = its[i];
	}
}

static int tw(const char *s) { return XTextWidth(fs, s, strlen(s)) + 8; }

static int pw(void) { return pr && *pr ? tw(pr) : 0; }

static void item(int i, int x, int y, int iw) {
	if(i == sel) {
		XSetForeground(dp, gc, col[3]);
		XFillRectangle(dp, pm, gc, x, y, iw, fh);
	}
	XSetForeground(dp, gc, col[i == sel ? 2 : 0]);
	XDrawString(dp, pm, gc, x + 4, y + fa, mt[i], strlen(mt[i]));
}

static void dr(void) {
	int x = 0, i, mx, cx, iw;
	XSetForeground(dp, gc, col[1]);
	XFillRectangle(dp, pm, gc, 0, 0, ww, wh);
	if(pw()) {
		x = pw();
		XSetForeground(dp, gc, col[3]);
		XFillRectangle(dp, pm, gc, 0, 0, x, fh);
		XSetForeground(dp, gc, col[2]);
		XDrawString(dp, pm, gc, 4, fa, pr, strlen(pr));
	}
	XSetForeground(dp, gc, col[0]);
	XDrawString(dp, pm, gc, x + 4, fa, tb, tl);
	cx = x + 4 + XTextWidth(fs, tb, tl);
	XDrawLine(dp, pm, gc, cx, 2, cx, fh - 2);
	if(ln > 0) {
		for(i = hoff; i < nm && i < hoff + ln; i++)
			item(i, 0, fh * (i - hoff + 1), ww);
	} else {
		for(i = hoff, mx = cx + 12; i < nm && mx < ww; i++) {
			item(i, mx, 0, iw = tw(mt[i]));
			mx += iw;
		}
	}
	XCopyArea(dp, pm, w, gc, 0, 0, ww, wh, 0, 0);
	XFlush(dp);
}

static void run(const char *cmd) {
	if(!*cmd) exit(1);
	XCloseDisplay(dp);
	printf("%s\n", cmd);
	exit(0);
}

static void scroll_to(int s) {
	int ox, mx, i;
	if(nm == 0) { sel = s; return; }
	if(s < 0) s = 0;
	if(s >= nm) s = nm - 1;
	if(ln > 0) {
		if(s < hoff) hoff = s;
		else if(s >= hoff + ln) hoff = s - ln + 1;
		sel = s;
		return;
	}
	if(s < hoff) hoff = s;
	ox = pw() + 4 + XTextWidth(fs, tb, tl) + 12;
	while(1) {
		mx = ox;
		for(i = hoff; i < nm; i++)
			if((mx += tw(mt[i])) > ww) break;
		if(s < i) break;
		hoff++;
	}
	sel = s;
}

static void mv(int d) {
	if(sel + d >= 0 && sel + d < nm) { scroll_to(sel + d); dr(); }
}

static void kp(XKeyEvent *e) {
	char buf[32] = {0};
	KeySym ks;
	XLookupString(e, buf, sizeof buf, &ks, NULL);
	if(e->state & ControlMask) switch(ks) {
	case XK_u: tl = 0; tb[0] = 0; fi(); dr(); return;
	case XK_w:
		while(tl && tb[tl-1] == ' ') tb[--tl] = 0;
		while(tl && tb[tl-1] != ' ') tb[--tl] = 0;
		fi(); dr(); return;
	case XK_k: mv(-1); return;
	case XK_j: mv(1); return;
	}
	switch(ks) {
	case XK_Escape: exit(1);
	case XK_Return: case XK_KP_Enter: run(nm > 0 ? mt[sel] : tb); return;
	case XK_BackSpace: if(tl) { tb[--tl] = 0; fi(); dr(); } return;
	case XK_Up: case XK_Left: mv(-1); return;
	case XK_Down: case XK_Right: mv(1); return;
	default:
		if(!iscntrl((unsigned char)buf[0]) && tl < MAXTEXT - 1) {
			tb[tl++] = buf[0];
			tb[tl] = 0;
			fi(); dr();
		}
	}
}

static void nap(long ns) { struct timespec ts = {0, ns}; nanosleep(&ts, NULL); }

static void grabfocus(void) {
	XSetInputFocus(dp, w, RevertToParent, CurrentTime);
	XSync(dp, 0);
}

int main(int argc, char *argv[]) {
	int i;
	XSetWindowAttributes wa;
	XEvent ev;
	for(i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-b")) top = 0;
		else if(!strcmp(argv[i], "-l") && i+1 < argc) ln = atoi(argv[++i]);
		else if(!strcmp(argv[i], "-p") && i+1 < argc) pr = argv[++i];
	}
	rd();
	if(!(dp = XOpenDisplay(NULL))) return 1;
	scr = DefaultScreen(dp);
	root = RootWindow(dp, scr);
	sh = DisplayHeight(dp, scr);
	if(!(fs = XLoadQueryFont(dp, FONT)) && !(fs = XLoadQueryFont(dp, "fixed"))) return 1;
	fa = fs->ascent;
	fh = fs->ascent + fs->descent + 2;
	ww = DisplayWidth(dp, scr);
	wh = ln > 0 ? fh * (ln + 1) : fh;
	col[0] = xc(FGNORM); col[1] = xc(BGNORM);
	col[2] = xc(FGSEL);  col[3] = xc(BGSEL);
	wa.override_redirect = 1;
	wa.background_pixel = col[1];
	wa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;
	w = XCreateWindow(dp, root, 0, top ? 0 : sh - wh, ww, wh, 0,
		DefaultDepth(dp, scr), CopyFromParent, DefaultVisual(dp, scr),
		CWOverrideRedirect | CWBackPixel | CWEventMask, &wa);
	pm = XCreatePixmap(dp, w, ww, wh, DefaultDepth(dp, scr));
	if(pm == None) return 1;
	gc = XCreateGC(dp, w, 0, NULL);
	if(!gc) return 1;
	XSetFont(dp, gc, fs->fid);
	fi();
	XMapRaised(dp, w);
	XSync(dp, 0);
	for(i = 0; i < 1000; i++) {
		if(XGrabKeyboard(dp, root, 1, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess) break;
		nap(1000000);
	}
	if(i == 1000) return 1;
	grabfocus();
	dr();
	while(!XNextEvent(dp, &ev)) {
		if(ev.type == KeyPress) kp(&ev.xkey);
		else if(ev.type == Expose && !ev.xexpose.count) dr();
		else if(ev.type == DestroyNotify && ev.xdestroywindow.window == w) exit(1);
	}
	return 0;
}
