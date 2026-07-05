PREFIX = /usr/local

CC      = cc
CFLAGS  = -std=c99 -Os
LDFLAGS = -lX11

all: tmenu

tmenu: tmenu.c
	$(CC) $(CFLAGS) -o tmenu tmenu.c $(LDFLAGS)

clean:
	rm -f tmenu

install: all
	cp -f tmenu $(DESTDIR)$(PREFIX)/bin/tmenu
	cp -f tmenu_run $(DESTDIR)$(PREFIX)/bin/tmenu_run
	chmod 755 $(DESTDIR)$(PREFIX)/bin/tmenu $(DESTDIR)$(PREFIX)/bin/tmenu_run

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tmenu
	rm -f $(DESTDIR)$(PREFIX)/bin/tmenu_run

.PHONY: all clean install uninstall
