CC=gcc
CFLAGS=-O2 -std=gnu11

playerctl-scroller: playerctl-scroller.c
	$(CC) -o playerctl-scroller playerctl-scroller.c $(CFLAGS) && \
	cp ./playerctl-scroller /usr/local/bin/

clean:
	rm -f ./playerctl-scroller

uninstall:
	rm /usr/local/bin/playerctl-scroller
