//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_NET_IMP_H
#define GST_PLAY_NET_IMP_H

#include <glib.h>

typedef struct {
    gchar *match_id;
    void *function;
} callback_t;

char * mount_cb(gpointer userdata, char *mount);

char * unmount_cb(gpointer userdata, char *mount);

char * player_next_cb(gpointer userdata, char *cmd);

char *player_prev_cb(gpointer userdata, char *cmd);

char *player_play_cb(gpointer userdata, char *cmd);

char *player_pause_cb(gpointer userdata, char *cmd) ;

const callback_t * net_impl_cbs();

#endif //GST_PLAY_NET_IMP_H
