//
// Created by bingo on 18-7-28.
//

#include "play_list.h"
#include <string.h>
#include <pthread.h>

typedef struct _play_list_t {
    GList *songs;
    GList *iterator;
    pthread_mutex_t lock;
} play_list_t;

typedef struct _song_t {
    gchar *file_name;
    gchar *artist;
    gchar *title;
    gchar *album;
} song_t;


static void song_free(gpointer data);

play_list_t *play_list_new() {
    song_t *ptr_song = g_new0(song_t, 1);
    play_list_t *play = g_new(play_list_t, 1);
    if (ptr_song == NULL || play == NULL) {
        return NULL;
    }
    play->songs = g_list_append(NULL, ptr_song);
    play->iterator = play->songs;
    ptr_song->file_name = g_strdup("000sentinel");
    play->iterator = play->songs;
    pthread_mutex_init(&play->lock, NULL);
    return play;
}


static gint compare(gconstpointer a, gconstpointer b) {
    gchar *a1, *b1;
    gint ret;
    a1 = g_utf8_collate_key_for_filename(((song_t *) a)->file_name, -1);
    b1 = g_utf8_collate_key_for_filename(((song_t *) b)->file_name, -1);
    ret = strcmp(a1, b1);
    g_free(a1);
    g_free(b1);
    return ret;
}

play_list_t *
play_list_add_song(play_list_t *play_list, gchar *file_name, gchar *artist, gchar *title, gchar *album) {
    song_t *ptr_song = g_new0(song_t, 1);
    pthread_mutex_lock(&play_list->lock);
    g_assert(file_name != NULL);
    ptr_song->file_name = g_strdup(file_name);
    if (artist)
        ptr_song->artist = g_strdup(artist);
    if (title)
        ptr_song->title = g_strdup(title);
    if (album)
        ptr_song->album = g_strdup(album);
    play_list->songs = g_list_insert_sorted(play_list->songs, ptr_song, compare);
    pthread_mutex_unlock(&play_list->lock);
    return play_list;
}

static gboolean play_list_remove_compare(gpointer userdata, gchar *name) {
    song_t *s = (song_t *) userdata;
    if (!strncmp(s->file_name, name, strlen(name))) {
        return TRUE;
    }
    return FALSE;
}

gchar *play_list_remove_by_name(play_list_t *play_list, gchar *name) {
    pthread_mutex_lock(&play_list->lock);
    GList **pinterator = &play_list->iterator;
    while (*pinterator && play_list_remove_compare((*pinterator)->data, name)) {
        song_free((*pinterator)->data);
        GList *next = (*pinterator)->next;
        play_list->songs = g_list_delete_link(play_list->songs, *pinterator);
        *pinterator = next;
    }
    GList *iterator = play_list->songs;
    while (iterator) {
        if (play_list_remove_compare(iterator->data, name)) {
            song_free(iterator->data);
            GList *next = iterator->next;
            play_list->songs =g_list_delete_link(play_list->songs, iterator);
            iterator = next;
            continue;
        }
        iterator = iterator->next;
    }
    pthread_mutex_unlock(&play_list->lock);

}

static void song_free(gpointer data) {
    song_t *ptr_song = (song_t *) data;
    if (ptr_song->file_name) {
        g_free(ptr_song->file_name);
        ptr_song->file_name = NULL;
    }
    if (ptr_song->artist) {
        g_free(ptr_song->artist);
        ptr_song->artist = NULL;
    }
    if (ptr_song->title) {
        g_free(ptr_song->title);
        ptr_song->title = NULL;
    }
    if (ptr_song->album) {
        g_free(ptr_song->album);
        ptr_song->album = NULL;
    }
    g_free(ptr_song);
    ptr_song = NULL;
}


void play_list_destroy(play_list_t **play_list) {
    g_list_free_full((*play_list)->songs, song_free);
    pthread_mutex_destroy(&(*play_list)->lock);
    g_free(*play_list);
    *play_list = NULL;
}

gchar *play_list_next(play_list_t *play_list) {
    pthread_mutex_lock(&play_list->lock);
    if (g_list_length(play_list->songs) == 1) {
        pthread_mutex_unlock(&play_list->lock);
        return NULL;
    }
    if (!play_list->iterator) {
        play_list->iterator = play_list->songs;
    }
    if (play_list->iterator == play_list->songs) {
        play_list->iterator = play_list->songs->next;
    }
    gchar *pch = ((song_t *) play_list->iterator->data)->file_name;
    play_list->iterator = play_list->iterator->next;
    pthread_mutex_unlock(&play_list->lock);
    return pch;
}

gchar *play_list_pre(play_list_t *play_list) {
    pthread_mutex_lock(&play_list->lock);
    if (g_list_length(play_list->songs) == 1) {
        pthread_mutex_unlock(&play_list->lock);
        return NULL;
    }
    if (!play_list->iterator) {
        play_list->iterator = play_list->songs;
    }
//当前是第一个
    if (!play_list->iterator->prev) {
        play_list->iterator = g_list_last(play_list->songs);
    } else if (play_list->iterator->prev == play_list->songs) {
//当前是第二个
        play_list->iterator = g_list_last(play_list->songs);
    } else {
        play_list->iterator = play_list->iterator->prev;
    }
    gchar *pch = ((song_t *) play_list->iterator->data)->file_name;
    pthread_mutex_unlock(&play_list->lock);
    return pch;
}

static void show(gpointer data, gpointer user_data) {
    song_t *song = (song_t *) data;
    g_print("---%s   %p \n", song->file_name, data);
}

void play_list_show(play_list_t *play_list) {
    pthread_mutex_lock(&play_list->lock);
    g_list_foreach(play_list->songs, show, NULL);
    pthread_mutex_unlock(&play_list->lock);
}


gboolean play_list_is_empty(play_list_t *play_list) {
    int i = 0;
    //第一个元素是哨兵，避免代码里面对空的判断
    for (GList *iterator = play_list->songs;
         iterator && i < 2; iterator = iterator->next) {
        i++;
    }
    if (i == 2) {
        return FALSE;
    }
    return TRUE;
}