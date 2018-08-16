//
// Created by bingo on 18-7-28.
//

#include "play_list.h"
#include <string.h>
#include <pthread.h>
#include <gst/gstprotection.h>

static void song_free(gpointer data);



typedef struct _play_list_t {
	GList *songs;
	GList *iterator;
	pthread_mutex_t lock;
//    后续修改为读写锁
} play_list_t;

typedef struct _song_t {
	gchar *file_name;
	gchar *artist;
	gchar *title;
	gchar *album;
} song_t;

gboolean play_list_get_cur_offset(play_list_t *play_list, gchar *buf) {
	int ret = 0;
	ret = g_list_position(play_list->songs, play_list->iterator);
	sprintf(buf, "%d", ret);
	return TRUE;
}

gboolean play_list_get_totals_songs(play_list_t *play_list, gchar *buf) {
	sprintf(buf, "%d", g_list_length(play_list->songs) -1);
	return TRUE;
}

gboolean play_list_get_artist(play_list_t *play_list, gchar *buf) {
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->artist) {
			sprintf(buf, "%s", song->artist);
			return TRUE;
		}
	}
	return FALSE;
}

gboolean play_list_get_title(play_list_t *play_list, gchar *buf) {
	char * path= NULL;
	char * file=NULL;
	if (play_list->iterator) {
		song_t *song = (song_t *) (play_list->iterator->data);
		if(song->file_name){
			path = gst_uri_get_location(song->file_name);
			file = strrchr(path, '/') +1;
			sprintf(buf, "%s", file);
			free(path);
			return TRUE;
		}
	}
	return FALSE;
}

gboolean play_list_get_album(play_list_t *play_list, gchar *buf) {
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->album) {
			sprintf(buf, "%s", song->album);
			return TRUE;
		}
	}
	return FALSE;

}

int play_list_get_file_names(play_list_t *play_list, gint min, gint max, gchar *buf,const gchar *prefix) {
//	play_list->songs->prev->next = NULL;
	int offset = 0;
	int i ;
	GList *itr = g_list_nth(play_list->songs, min);
	for (i = min; itr && i <= max; i++, itr = itr->next) {
		char * url = ((song_t *) (itr->data))->file_name;
		char * path = gst_uri_get_location(url);
		char * file = strrchr(path, '/') +1;
        sprintf(buf+offset, "%s=%d:%s", prefix,i, file);
		printf("%s\n",buf +offset);
		offset +=  strlen(buf +offset) +1;
		free(path);
	}
    return offset+1;
}

void play_list_set_artist(play_list_t *play_list, gchar *buf){
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->artist) {
			g_free(song->artist);
		}
		song->artist = buf;
	}
}

void  play_list_set_title(play_list_t *play_list, gchar *buf){
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->title) {
			g_free(song->title);
		}
		song->title = buf;
	}
}

void play_list_set_album(play_list_t *play_list, gchar *buf){
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->album) {
			g_free(song->album);
		}
		song->album = buf;
	}
}

void play_list_clear_titil_album_artist(play_list_t *play_list){
	if (play_list->iterator) {
		song_t *song = (song_t *) play_list->iterator->data;
		if (song->album) {
			g_free(song->album);
		}
		if (song->title) {
			g_free(song->title);
		}
		if (song->artist) {
			g_free(song->artist);
		}
		song->album = song->artist = song->title = NULL;
	}
}




play_list_t *play_list_new(void) {
	song_t *ptr_song = g_new0(song_t, 1);
	play_list_t *play = g_new(play_list_t, 1);
	if (ptr_song == NULL || play == NULL) {
		return NULL;
	}
	play->songs = g_list_append(NULL, ptr_song);
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

void play_list_remove_by_name(play_list_t *play_list, gchar *name) {
	GList **pinterator = NULL;
	GList *next = NULL;
	GList *iterator =NULL;
	pthread_mutex_lock(&play_list->lock);
	pinterator = &play_list->iterator;
	while (*pinterator && play_list_remove_compare((*pinterator)->data, name)) {
		song_free((*pinterator)->data);
		next = (*pinterator)->next;
		play_list->songs = g_list_delete_link(play_list->songs, *pinterator);
		*pinterator = next;
	}
	iterator = play_list->songs;
	while (iterator) {
		if (play_list_remove_compare(iterator->data, name)) {
			song_free(iterator->data);
			next = iterator->next;
			play_list->songs = g_list_delete_link(play_list->songs, iterator);
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
	gchar *pch = NULL;
	pthread_mutex_lock(&play_list->lock);
	if (g_list_length(play_list->songs) == 1) {
		pthread_mutex_unlock(&play_list->lock);
		return NULL;
	}
	play_list_clear_titil_album_artist(play_list);
	if (!play_list->iterator) {
		play_list->iterator = play_list->songs;
	}
	if (play_list->iterator == play_list->songs) {
		play_list->iterator = play_list->songs->next;
	}

	pch = ((song_t *) play_list->iterator->data)->file_name;
	play_list->iterator = play_list->iterator->next;
	pthread_mutex_unlock(&play_list->lock);
	return pch;
}

gchar *play_list_pre(play_list_t *play_list) {
	gchar *pch = NULL;
	pthread_mutex_lock(&play_list->lock);
	if (g_list_length(play_list->songs) == 1) {
		pthread_mutex_unlock(&play_list->lock);
		return NULL;
	}
	play_list_clear_titil_album_artist(play_list);
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
	pch = ((song_t *) play_list->iterator->data)->file_name;
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
	GList *iterator = NULL;
	//第一个元素是哨兵，避免代码里面对空的判断
	for (iterator = play_list->songs;
		 iterator && i < 2; iterator = iterator->next) {
		i++;
	}
	if (i == 2) {
		return FALSE;
	}
	return TRUE;
}
