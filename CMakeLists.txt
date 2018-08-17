cmake_minimum_required(VERSION 3.12)
project(player C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(player
        include/bk.h
        include/libue.h
        include/net_impl.h
        include/play_list.h
        include/player.h
        include/protocol.h
        include/usb_scan.h
        src/main.c
        src/net_impl.c
        src/play_list.c
        src/usb_scan_by_socket.c
        src/player_opt.h include/player_notiy.h)

target_link_libraries(player -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -pthread  -lgstaudio-1.0 -lgstbase-1.0 )
include_directories(
        /usr/include/gstreamer-1.0
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include)
