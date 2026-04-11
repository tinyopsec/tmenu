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
	chmod 755 $(DESTDIR)$(PREFIX)/bin/tmenu

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/tmenu

.PHONY: all clean install uninstall
