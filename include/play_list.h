//
// Created by bingo on 18-7-28.
//

#ifndef GST_PLAY_PLAY_LIST_H
#define GST_PLAY_PLAY_LIST_H

#include <glib/gstdio.h>

typedef struct _play_list_t play_list_t;
typedef struct _song_t song_t;

play_list_t *play_list_add_song(play_list_t *play_list, gchar *file_name, gchar *artist, gchar *title, gchar *album);

void play_list_remove_by_name(play_list_t *play_list, gchar *name);

play_list_t *play_list_new(void);

void play_list_destroy(play_list_t **play_list);

gchar *play_list_next(play_list_t *play_list);

gchar *play_list_pre(play_list_t *play_list);

void play_list_show(play_list_t *play_list);

gboolean play_list_is_empty(play_list_t *play_list);

gboolean play_list_get_artist(play_list_t *play_list, gchar *buf);

gboolean play_list_get_title(play_list_t *play_list, gchar *buf);

gboolean play_list_get_album(play_list_t *play_list, gchar *buf);

gboolean play_list_get_totals_songs(play_list_t *play_list, gchar *buf);

gboolean play_list_get_cur_offset(play_list_t *play_list, gchar *buf);

void play_list_set_artist(play_list_t *play_list, gchar *buf);

void play_list_set_title(play_list_t *play_list, gchar *buf);

void play_list_set_album(play_list_t *play_list, gchar *buf);

void play_list_clear_titil_album_artist(play_list_t *play_list);

int  play_list_get_file_names(play_list_t *play_list, gint min, gint max, gchar *buf,const gchar *prefix);

#endif //GST_PLAY_PLAY_LIST_H
