//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_PLAYER_H
#define GST_PLAY_PLAYER_H


#include <glib.h>
#include <gstreamer-1.0/gst/gst.h>

#include "play_list.h"
#include "socket_observer.h"
#include "player_opt.h"


typedef struct _gst_data {
    play_list_t *play_list;
    GstElement *playbin;
    GMainLoop *loop;
    socket_observer_t *usb_monitor;
    player_opt_t player_opt;
    GstBus *bus;
} gst_data_t;


char *player_query_duration(gst_data_t *pdata, char *buf);

char *player_query_position(gst_data_t *pdata, char *buf);

char *player_query_status(gst_data_t *pdata, char *buf);

char *player_query_abulum(gst_data_t *pdata, char *buf);

int   player_query_file_names(gst_data_t *pdata, int min, int max, char *buf,const char *prefix);

char *player_query_title(gst_data_t *pdata, char *buf);

char *player_query_singer(gst_data_t *pdata, char *buf);

char *player_query_totals_songs(gst_data_t *pdata, char *buf);

char *player_query_cur_offset(gst_data_t *pdata, char *buf);

void player_init(gst_data_t *data);

gboolean new_player(gst_data_t *pdata);

void player_destory(gst_data_t *pdata) ;

player_opt_t * player_get_opt(gst_data_t *pdata);

void player_runing(gst_data_t *data);


#endif //GST_PLAY_PLAYER_H
