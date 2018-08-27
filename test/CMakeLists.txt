
add_executable(test_opt  g_test_player_opt.c)
target_link_libraries(test_opt -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -pthread  -lgstaudio-1.0 -lgstbase-1.0 )
include_directories(
        /usr/include/gstreamer-1.0
        /usr/include/glib-2.0
        /usr/lib/x86_64-linux-gnu/glib-2.0/include)
