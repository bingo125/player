cmake_minimum_required(VERSION 3.10)

project(gst_play C)
set(CMAKE_C_STANDARD 90)

include_directories(
        /usr/include/gstreamer-1.0
        /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include
        include)
#add_executable(gst_play  test/socket_test.c src/usb_scan_by_socket.c)
#add_executable(gst_play main.c songs.c songs.h test_play_list.c)


#add_executable(gst_play  test/play_list_test.c  src/play_list.c)

add_definitions(-Wall -Werror -g -Wmissing-prototypes -Wdeclaration-after-statement -Wredundant-decls -Wdiscarded-qualifiers -Wdiscarded-qualifiers)


#正式版本编译打开
add_executable(gst_play src/main.c   src/play_list.c src/net_impl.c src/usb_scan_by_socket.c )
#add_executable(gst_play test/input_test.c)
target_link_libraries(gst_play gstaudio-1.0 gstbase-1.0 gstreamer-1.0 gobject-2.0 glib-2.0 pthread)
