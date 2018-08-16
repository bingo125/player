.PHONY: player
all: player

srcs += src/main.c src/net_impl.c src/play_list.c src/usb_scan_by_socket.c 
# CC = ${CROSS_COMPILE}gcc
#ifeq ($(TARGET_ARCH), arm)
#ifeq ($(TARGET_ARCH), arm)
# 暂时替换掉
incs = -I/home/bingo/workspace/include/gstreamer-1.0
incs += -Iinclude
incs += -I/home/bingo/workspace/include/glib-2.0
incs += -I/home/bingo/workspace/include
#incs += -I/home/bingo/usr
#incs += -I/home/bingo/usr/include/glib-2.0
libs += -L/home/bingo/workspace/libs  -lpthread -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -lgmodule-2.0 -lffi -lpcre -lgstaudio-1.0 -Wl,-rpath /home/bingo/workspace/libs
# libs += -L/home/bingo/workspace/libs -lgstreamer-1.0  -lglib-2.0
# LDFLAGS +=  
flags = -Wno-shift-count-overflow -g 
# flags = -Wall -Wshadow  -Werror -Wundef -Wno-unused-but-set-variable  

# player: test.c
player: ${srcs}
	${CC}  ${flags} ${incs}  ${libs} $^ -o $@

# player : fileScaner.c
# 	gcc $^ -o $@ `pkg-config --cflags --libs gstreamer-1.0`
#else
#player : ${srcs}
#	echo "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#	gcc $^ -o $@ `pkg-config --cflags --libs gstreamer-1.0 gstreamer-audio-1.0` -g
#endif
