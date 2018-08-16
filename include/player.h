//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_PLAYER_H
#define GST_PLAY_PLAYER_H

#include <glib.h>
typedef struct _gst_data gst_data;

void player_list_add(gst_data *pdata, const char *dir_name);

void player_list_remove(gst_data *pdata, const char *dir_name);

void volume_down(gst_data *pdata);

void volume_up(gst_data *pdata);

void player_next(gst_data *pdata);

void player_toggle_play_pause(gst_data *pdata);

void player_prev(gst_data *pdata);

void player_play(gst_data *pdata);

void player_pause(gst_data *pdata);

char *play_query_duration(gst_data *pdata, char *buf);

char *play_query_position(gst_data *pdata, char *buf);

char *play_query_status(gst_data *pdata, char *buf);

char *play_query_abulum(gst_data *pdata, char *buf);

int play_query_file_names(gst_data *pdata, int min, int max, char *buf,const char *prefix);

char *play_query_title(gst_data *pdata, char *buf);

char *play_query_singer(gst_data *pdata, char *buf);

char *play_query_totals_songs(gst_data *pdata, char *buf);

char *play_query_cur_offset(gst_data *pdata, char *buf);

#endif //GST_PLAY_PLAYER_H
