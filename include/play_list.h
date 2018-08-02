//
// Created by bingo on 18-7-28.
//

#ifndef GST_PLAY_PLAY_LIST_H
#define GST_PLAY_PLAY_LIST_H


#include <glib/gstdio.h>

typedef struct _play_list_t play_list_t;
typedef struct _song_t song_t;

play_list_t *play_list_add_song(play_list_t *play_list, gchar *file_name, gchar *artist, gchar *title, gchar *album);

gchar *play_list_remove_by_name(play_list_t *play_list, gchar *name);

play_list_t *play_list_new();

void play_list_destroy(play_list_t **play_list);

gchar *play_list_next(play_list_t *play_list);

gchar *play_list_pre(play_list_t *play_list);

void play_list_show(play_list_t *play_list);

gboolean play_list_is_empty(play_list_t *play_list);

#endif //GST_PLAY_PLAY_LIST_H
