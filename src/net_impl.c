//
// Created by bingo on 18-7-31.
//

#include <memory.h>
#include "net_impl.h"
#include "player.h"

void mount_cb(gpointer userdata, char *mount) {

    g_print("mount:%s\n", mount);
    player_list_add(userdata, mount);
    player_next_cb(userdata);
}

void unmount_cb(gpointer userdata, char *mount) {
    g_print("unmount_cb:%s\n", mount);
    player_list_remove(userdata, mount);
}

void player_next_cb(gpointer userdata) {
    g_print("player_next_cb:\n");
    player_next(userdata);
}

void player_prev_cb(gpointer userdata) {
    g_print("player_prev_cb:\n");
    player_prev(userdata);
}

void player_play_cb(gpointer userdata) {
    g_print("player_play_cb:\n");
    player_play(userdata);
}
void player_pause_cb(gpointer userdata) {
    g_print("player_pause_cb:\n");
    player_pause(userdata);
}



#define MOUNT "VOLUME MOUNT="
#define UNMOUNT "VOLUME UNMOUNT="
#define NEXT "PLAYER NEXT"
#define PREV "PLAYER PREV"
#define PLAY "PLAYER PLAY"
#define PAUSE "PLAYER PAUSE"



callback_t callbacks[] = {
        {
                .match_id =MOUNT,
                .n_args = 1,
                .function = mount_cb,
        },
        {
                .match_id = UNMOUNT,
                .n_args = 1,
                .function = unmount_cb,
        },
        {
                .match_id = NEXT,
                .n_args = 0,
                .function = player_next_cb,
        },
        {
                .match_id = PREV,
                .n_args = 0,
                .function = player_prev_cb,
        },
        {
                .match_id = PLAY,
                .n_args = 0,
                .function = player_play_cb,
        },
        {
                .match_id = PAUSE,
                .n_args = 0,
                .function = player_pause_cb
                        ,
        },
        {
                .match_id = NULL,
                .n_args = 0,
                .function = NULL,
        },
};

const callback_t * net_impl_cbs(){
    return callbacks;
}