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
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define MAXITEMS 65536
#define MAXTEXT  256
#define BUFSZ    4096

typedef struct { char *s; } It;

static Display *dp;
static Window w, root;
static int scr, sw, sh, wh, ww;
static GC gc;
static Pixmap pm;
static XFontStruct *fs;
static int fa, fh;
static It its[MAXITEMS];
static int ni = 0;
static It *mt[MAXITEMS];
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
	char *p;
	while(fgets(buf, sizeof buf, stdin)) {
		int l = strlen(buf);
		if(l && buf[l-1] == '\n') buf[--l] = 0;
		if(!l) continue;
		p = strdup(buf);
		if(!p) exit(1);
		its[ni].s = p;
		if(++ni >= MAXITEMS) break;
	}
}

static void fi(void) {
	int i, p, q;
	char lo[MAXTEXT], tmp[BUFSZ];
	sel = 0; hoff = 0;
	for(i = 0; tb[i]; i++) lo[i] = tolower((unsigned char)tb[i]);
	lo[i] = 0;
	if(!tl) {
		for(nm = 0; nm < ni;) mt[nm] = &its[nm], nm++;
		return;
	}
	p = 0; q = 0;
	for(i = 0; i < ni; i++) {
		int j;
		for(j = 0; its[i].s[j]; j++) tmp[j] = tolower((unsigned char)its[i].s[j]);
		tmp[j] = 0;
		if(!strstr(tmp, lo)) continue;
		if(strncmp(tmp, lo, tl) == 0) mt[p++] = &its[i];
		else mt[MAXITEMS/2 + q++] = &its[i];
	}
	for(i = 0; i < q; i++) mt[p + i] = mt[MAXITEMS/2 + i];
	nm = p + q;
}

static void dr(void) {
	int x = 0, y, i, mx, cx;
	XSetForeground(dp, gc, col[1]);
	XFillRectangle(dp, pm, gc, 0, 0, ww, wh);
	if(pr && *pr) {
		int pl = strlen(pr);
		int pw = XTextWidth(fs, pr, pl) + 8;
		XSetForeground(dp, gc, col[3]);
		XFillRectangle(dp, pm, gc, 0, 0, pw, fh);
		XSetForeground(dp, gc, col[2]);
		XDrawString(dp, pm, gc, 4, fa, pr, pl);
		x = pw;
	}
	XSetForeground(dp, gc, col[0]);
	XDrawString(dp, pm, gc, x + 4, fa, tb, tl);
	cx = x + 4 + XTextWidth(fs, tb, tl);
	XDrawLine(dp, pm, gc, cx, 2, cx, fh - 2);
	if(ln > 0) {
		for(i = 0; i < nm && i < ln; i++) {
			int sl = strlen(mt[i]->s);
			y = fh + fh * i;
			if(i == sel) {
				XSetForeground(dp, gc, col[3]);
				XFillRectangle(dp, pm, gc, 0, y, ww, fh);
				XSetForeground(dp, gc, col[2]);
			} else {
				XSetForeground(dp, gc, col[0]);
			}
			XDrawString(dp, pm, gc, 4, y + fa, mt[i]->s, sl);
		}
	} else {
		mx = cx + 12;
		for(i = hoff; i < nm && mx < ww; i++) {
			int sl = strlen(mt[i]->s);
			int iw = XTextWidth(fs, mt[i]->s, sl) + 8;
			if(i == sel) {
				XSetForeground(dp, gc, col[3]);
				XFillRectangle(dp, pm, gc, mx, 0, iw, fh);
				XSetForeground(dp, gc, col[2]);
			} else {
				XSetForeground(dp, gc, col[0]);
			}
			XDrawString(dp, pm, gc, mx + 4, fa, mt[i]->s, sl);
			mx += iw;
		}
	}
	XCopyArea(dp, pm, w, gc, 0, 0, ww, wh, 0, 0);
	XFlush(dp);
}

static void run(const char *cmd) {
	pid_t pid;
	if(!cmd || !*cmd) exit(1);
	pid = fork();
	if(pid < 0) exit(1);
	if(pid == 0) {
		XCloseDisplay(dp);
		execl("/bin/sh", "sh", "-c", cmd, NULL);
		_exit(1);
	}
	exit(0);
}

static void scroll_to(int s) {
	int mx, i, cx2;
	if(ln > 0 || nm == 0) { sel = s; return; }
	if(s < 0) s = 0;
	if(s >= nm) s = nm - 1;
	if(s < hoff) hoff = s;
	cx2 = (pr && *pr ? XTextWidth(fs, pr, strlen(pr)) + 8 : 0)
	      + 4 + XTextWidth(fs, tb, tl) + 12;
	while(1) {
		mx = cx2;
		for(i = hoff; i < nm; i++) {
			int iw = XTextWidth(fs, mt[i]->s, strlen(mt[i]->s)) + 8;
			if(mx + iw > ww) break;
			mx += iw;
		}
		if(s < i) break;
		hoff++;
	}
	sel = s;
}

static void kp(XKeyEvent *e) {
	char buf[32];
	KeySym ks;
	XLookupString(e, buf, sizeof buf, &ks, NULL);
	if(e->state & ControlMask) {
		switch(ks) {
		case XK_u: tl = 0; tb[0] = 0; fi(); dr(); return;
		case XK_w:
			while(tl && tb[tl-1] == ' ') tb[--tl] = 0;
			while(tl && tb[tl-1] != ' ') tb[--tl] = 0;
			fi(); dr(); return;
		case XK_k:
			if(sel < nm - 1) { scroll_to(sel + 1); dr(); }
			return;
		case XK_j:
			if(sel > 0) { scroll_to(sel - 1); dr(); }
			return;
		}
	}
	switch(ks) {
	case XK_Escape: exit(1);
	case XK_Return:
	case XK_KP_Enter:
		run(nm > 0 ? mt[sel]->s : tb);
		break;
	case XK_BackSpace:
		if(tl) { tb[--tl] = 0; fi(); dr(); }
		return;
	case XK_Up:
	case XK_Left:
		if(sel > 0) { scroll_to(sel - 1); dr(); }
		return;
	case XK_Down:
	case XK_Right:
		if(sel < nm - 1) { scroll_to(sel + 1); dr(); }
		return;
	default:
		if(buf[0] && !iscntrl((unsigned char)buf[0]) && tl < MAXTEXT - 1) {
			tb[tl++] = buf[0];
			tb[tl] = 0;
			fi(); dr();
		}
	}
}

static void grabfocus(void) {
	struct timespec ts = {0, 10000000};
	Window fw;
	int di, i;
	for(i = 0; i < 100; i++) {
		XGetInputFocus(dp, &fw, &di);
		if(fw == w) return;
		XSetInputFocus(dp, w, RevertToParent, CurrentTime);
		XSync(dp, 0);
		nanosleep(&ts, NULL);
	}
}

int main(int argc, char *argv[]) {
	int i;
	XSetWindowAttributes wa;
	for(i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-b")) top = 0;
		else if(!strcmp(argv[i], "-l") && i+1 < argc) ln = atoi(argv[++i]);
		else if(!strcmp(argv[i], "-p") && i+1 < argc) pr = argv[++i];
	}
	rd();
	if(!(dp = XOpenDisplay(NULL))) return 1;
	scr = DefaultScreen(dp);
	root = RootWindow(dp, scr);
	sw = DisplayWidth(dp, scr);
	sh = DisplayHeight(dp, scr);
	if(!(fs = XLoadQueryFont(dp, FONT)))
		if(!(fs = XLoadQueryFont(dp, "fixed"))) return 1;
	fa = fs->ascent;
	fh = fs->ascent + fs->descent + 2;
	ww = sw;
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
	{
		struct timespec ts = {0, 1000000};
		for(i = 0; i < 1000; i++) {
			if(XGrabKeyboard(dp, root, 1, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess) break;
			nanosleep(&ts, NULL);
		}
		if(i == 1000) return 1;
	}
	grabfocus();
	dr();
	{
		XEvent ev;
		while(!XNextEvent(dp, &ev))
			if(ev.type == KeyPress) kp(&ev.xkey);
			else if(ev.type == Expose && !ev.xexpose.count) dr();
			else if(ev.type == DestroyNotify && ev.xdestroywindow.window == w) exit(1);
	}
	return 0;
}
