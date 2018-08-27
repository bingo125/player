cmake_minimum_required(VERSION 3.12)
project(player C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(
        /usr/include/gstreamer-1.0
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include)

add_definitions(-Wall -Werror -g)

add_executable(player
        include/net_impl.h
        include/play_list.h
        include/player.h
        include/protocol.h
        include/socket_observer.h
        src/player.c
        src/net_impl.c
        src/play_list.c
        src/socket_observer.c
        include/player_opt.h
        include/player_notiy.h
        src/player_opt.c
        src/main.c
        )

target_link_libraries(player -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -pthread  -lgstaudio-1.0 -lgstbase-1.0 )

add_subdirectory(test)


