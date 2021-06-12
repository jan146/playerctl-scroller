CC=gcc
CFLAGS=-std=gnu11 -D_XOPEN_SOURCE=600 -D_POSIX_C_SOURCE=200112L 

playerctl-scroller: playerctl-scroller.c
	$(CC) -o playerctl-scroller playerctl-scroller.c cscroll.c $(CFLAGS) && \
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
	$(CC) -o playerctl-scroller playerctl-scroller.c cscroll.c $(CFLAGS) && \
	cp ./playerctl-scroller /usr/local/bin/

compile:
	$(CC) -o playerctl-scroller playerctl-scroller.c cscroll.c $(CFLAGS) 
