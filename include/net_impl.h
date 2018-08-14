//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_NET_IMP_H
#define GST_PLAY_NET_IMP_H

#include <glib.h>

#define MOUNT "VOLUME MOUNT="
#define UNMOUNT "VOLUME UNMOUNT="

typedef struct _callback_t callback_t;

typedef char * (*func2)( callback_t*, char *,gpointer);

typedef struct _callback_t{
    gchar *match_id;
	gchar *freeback;
	func2 function;
	gboolean  muti_char;
} callback_t;

char * mount_cb(callback_t *cb ,char *cmd,gpointer userdata);

char * unmount_cb(callback_t *cb ,char *cmd,gpointer userdata);

char * player_next_cb(callback_t *cb ,char *cmd,gpointer userdata);

char *player_prev_cb(callback_t *cb ,char *cmd,gpointer userdata);

char *player_play_cb(callback_t *cb ,char *cmd,gpointer userdata);

char *player_pause_cb(callback_t *cb ,char *cmd,gpointer userdata) ;

char *player_info_cur_cb(callback_t *cb ,char *cmd,gpointer userdata);

char *player_info_duration_cb(callback_t *cb, char *cmd, gpointer userdata);

callback_t * net_impl_cbs();

//
void net_impl_set_buf(GList** buf);

#endif //GST_PLAY_NET_IMP_H
