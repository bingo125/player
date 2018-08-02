//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_NET_IMP_H
#define GST_PLAY_NET_IMP_H

#include <glib.h>

typedef struct {
    gchar *match_id;
    guint n_args;
    void *function;
} callback_t;

void mount_cb(gpointer userdata, char *mount);

void unmount_cb(gpointer userdata, char *mount);

void player_next_cb(gpointer userdata);

void player_prev_cb(gpointer userdata);

void player_play_cb(gpointer userdata);

const callback_t * net_impl_cbs();

#endif //GST_PLAY_NET_IMP_H
