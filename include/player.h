//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_PLAYER_H
#define GST_PLAY_PLAYER_H

#include <glib.h>
typedef struct _gst_data gst_data;


void player_list_add(gst_data *pdata, const char *dir_name) ;

void player_list_remove(gst_data *pdata, const char *dir_name) ;

void volume_down(gst_data *pdata) ;

void volume_up(gst_data *pdata) ;

void player_next(gst_data *pdata);

void player_prev(gst_data *pdata);

void player_play(gst_data *pdata);

void player_pause(gst_data *pdata);

#endif //GST_PLAY_PLAYER_H
