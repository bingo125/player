.PHONY: player
all: player

SRCS += src/main.c src/net_impl.c src/player.c src/player_opt.c src/play_list.c src/socket_observer.c 
CFLAGS+=`pkg-config --cflags gstreamer-1.0` -I./include -g -Wall -Werror
LDFLAGS+=`pkg-config --libs gstreamer-1.0`
player: ${SRCS}
	${CC} ${CFLAGS}   -o $@ $^ ${LDFLAGS}
