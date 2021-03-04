CC=gcc
CFLAGS=-O2 -std=gnu11

playerctl-scroller: playerctl-scroller.c
	$(CC) -o playerctl-scroller playerctl-scroller.c $(CFLAGS) && \
	cp ./playerctl-scroller /usr/local/bin/ 

config:
	mkdir -p ${HOME}/.config/polybar/scripts && \
	cp ./scroller.sh ${HOME}/.config/polybar/scripts/

clean:
	rm -f ./playerctl-scroller

uninstall:
	rm /usr/local/bin/playerctl-scroller

reinstall:
	rm -f ./playerctl-scroller && \
	rm /usr/local/bin/playerctl-scroller && \
	$(CC) -o playerctl-scroller playerctl-scroller.c $(CFLAGS) && \
	cp ./playerctl-scroller /usr/local/bin/

