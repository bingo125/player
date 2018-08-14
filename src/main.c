#include <fcntl.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gmodule.h>
#include <gstreamer-1.0/gst/gst.h>
#include <gst/audio/audio.h>
#include <gst/tag/tag.h>
#include <locale.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <net_impl.h>
#include "play_list.h"
#include "player.h"
#include "usb_scan.h"

#define BLOCK_ID_INVALID  ( -1 )

typedef enum {
    cust_list_empty,
    cust_list_have,
    cust_pause,
    cust_play,
} cust_state;

typedef struct _gst_data {
    play_list_t *play_list;
    GstElement *playbin;
    guint bus_watch;
    GMainLoop *loop;
    usb_monitor_t *usb_monitor;
    cust_state state;
} gst_data;

static void music_scan(gst_data *pdata, const gchar *dir_name);

static void music_song_remove(gst_data *pdata, const gchar *dir_name);

gchar music[][8] =
        {
                {".flac"},
                {".mp3"},
                {".wav"},
                {".ogg"},
        };

static gboolean music_compare(const gchar *name) {
    int sz = sizeof(music) / sizeof(music[0]);
    gint result = 0;
    for (int i = 0; i < sz; i++) {
        if (!(result = strcmp(name, music[i]))) {
            return TRUE;
        }
    }
    return FALSE;
}

void volume_down(gst_data *pdata) {
    gdouble volume;
    volume = gst_stream_volume_get_volume(GST_STREAM_VOLUME(pdata->playbin), GST_STREAM_VOLUME_FORMAT_LINEAR);
    volume -= 0.1;
    if (volume < 0.0)
        volume = 0;
    g_print(("Volume: %.0f%%"), volume * 100);
    g_print("                  \n");
    gst_stream_volume_set_volume(GST_STREAM_VOLUME(pdata->playbin), GST_STREAM_VOLUME_FORMAT_LINEAR, volume);
}

void volume_up(gst_data *pdata) {
    gdouble volume;
    volume = gst_stream_volume_get_volume(GST_STREAM_VOLUME(pdata->playbin), GST_STREAM_VOLUME_FORMAT_LINEAR);
    volume += 0.1;
    if (volume > 1.0)
        volume = 1.0;
    g_print(("Volume: %.0f%%"), volume * 100);
    g_print("                  \n");
    gst_stream_volume_set_volume(GST_STREAM_VOLUME(pdata->playbin), GST_STREAM_VOLUME_FORMAT_LINEAR, volume);
}

static void scan_dir(const gchar *dir_name, play_list_t *play_list) {
    GError *err = NULL;
    GDir *dir;
    const gchar *entry;
    GList *index, *dirList = NULL;
    dir = g_dir_open(dir_name, 0, &err);
    if (err != NULL) {
        g_printerr("%s", err->message);
        g_error_free(err);
        return;
    }
    while ((entry = g_dir_read_name(dir)) != NULL) {
        if (entry[0] == '.') {
            continue;
        }
        gchar *path = g_build_filename(dir_name, entry, NULL);
        if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
            dirList = g_list_append(dirList, path);
        } else if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            const gchar *c = entry;
            while (*c != '\0') {
                if (*c == '.' && music_compare(c)) {
                    gchar *ch = gst_filename_to_uri(path, NULL);
                    g_print("----ADD list----   %s \n", ch);
                    /* *fileList = g_list_append(*fileList,uri); */
                    play_list_add_song(play_list, ch, NULL, NULL, NULL);
                    /**fileList = g_list_append(NULL, uri);*/

                    break;
                }
                c++;
            }
            g_free(path);
        } else {
            g_free(path);
        }
    }
    g_dir_close(dir);
    for (index = dirList; index != NULL; index = index->next) {
        scan_dir(index->data, play_list);
        g_free(index->data);
    }
    g_list_free(dirList);
}

static void analyze_streams(gst_data *data) {
    gint i;
    GstTagList *tags;
    gchar *str;
    guint rate;
    g_print("analyze_streams \n");
    play_list_t * play_list = data->play_list;
    for (i = 0; i < 1; i++) {
        tags = NULL;
        /* Retrieve the stream's audio tags */
        g_signal_emit_by_name(data->playbin, "get-audio-tags", i, &tags);
        if (tags) {
            g_print("audio stream %d:\n", i);
			if (gst_tag_list_get_string(tags, GST_TAG_TITLE, &str)) {
				g_print("  GST_TAG_TITLE: %s\n", str);
                play_list_set_title(play_list, str);
			}
            if (gst_tag_list_get_string(tags, GST_TAG_AUDIO_CODEC, &str)) {
                g_print("  codec: %s\n", str);
                g_free(str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_ARTIST, &str)) {
                g_print("  ARTIST: %s\n", str);
                play_list_set_artist(play_list, str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_ALBUM, &str)) {
                g_print("  ALBUM: %s\n", str);
                play_list_set_album(play_list, str);
            }
            if (gst_tag_list_get_string(tags, GST_TAG_LANGUAGE_CODE, &str)) {
                g_print("  language: %s\n", str);
                g_free(str);
            }
            if (gst_tag_list_get_uint(tags, GST_TAG_BITRATE, &rate)) {
                g_print("  bitrate: %d\n", rate);
            }
            gst_tag_list_unref(tags);
        }
    }
}

static gboolean play_bus_msg(GstBus *bus, GstMessage *msg, gpointer user_data) {
    GError *err;
    gchar *debug_info;
    gst_data *pdata = (gst_data *) user_data;
    GstElement *playbin = pdata->playbin;

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element : %s\n", err->message);
            g_printerr("Debugging information: %s\n",
                       debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            gst_element_set_state(playbin, GST_STATE_NULL);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            play_list_clear_titil_album_artist(pdata->play_list);
            if (pdata->state == cust_play) {
                player_next(pdata);
            }
            /*player_prev(pdata);*/
            /* g_main_loop_quit(pdata->loop); */
            break;
        case GST_MESSAGE_STATE_CHANGED: {
            /* g_printf("GST_MESSAGE_STATE_CHANGED \n"); */
            GstState old_state, new_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);

//            g_print ("Pipeline state changed from %s to %s:  \n", gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));

            if (GST_STATE_PLAYING == new_state) {
                if (msg->src) {
                    gchar *pch = GST_OBJECT_NAME (msg->src);
                    g_print("%s\n", pch);
                    if(!strcmp(pch, "audio")){
						analyze_streams(pdata);
					}
                }
            }
            break;
//            if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pdata->playbin)) {
//                if (GST_STATE_PLAYING == new_state) {
//                    analyze_streams(pdata);
//                }
//                    volume_up(pdata);


        }
		default:
			break;
            /*default:*/
            /*g_printerr("Unexpected message received.\n");*/
    }
    return TRUE;
}

static gboolean new_player(gst_data *pdata) {
    guint bus_watch;
    GstBus *bus;
    GstElement *playbin = NULL;
    playbin = gst_element_factory_make("playbin", "audio");
    if (playbin == NULL) {
        g_printerr("failure to malloc the playbin");
        return FALSE;
    }
    bus = gst_element_get_bus(playbin);
    bus_watch = gst_bus_add_watch(bus, play_bus_msg, pdata);
    gst_object_unref(bus);
    pdata->playbin = playbin;
    pdata->bus_watch = bus_watch;
    return TRUE;
}

static void play_uri(GstElement *playbin, play_list_t *play_list, cust_state *p_state, gboolean isNext) {
    gchar *ch;
    if (isNext) {
        ch = play_list_next(play_list);
    } else {
        ch = play_list_pre(play_list);
    }
    if (ch != NULL) {
        g_printf("play uri ------%s \n", ch);
        gst_element_set_state(playbin, GST_STATE_READY);
        g_object_set(playbin, "uri", ch, NULL);
        gst_element_set_state(playbin, GST_STATE_PAUSED);
        if (*p_state == cust_play) {
            g_print("----------------\n");
            gst_element_set_state(playbin, GST_STATE_PLAYING);
        }
    } else {
        *p_state = cust_list_empty;
    }
}

static void play(gst_data *pdata, gboolean isNext) {
    GstElement *playbin = pdata->playbin;
    play_uri(playbin, pdata->play_list, &pdata->state, isNext);
}

void player_prev(gst_data *pdata) {
    if (pdata->state != cust_list_empty) {
        pdata->state = cust_play;
        play(pdata, FALSE);
    }
}

void player_next(gst_data *pdata) {

    if (pdata->state != cust_list_empty) {
        pdata->state = cust_play;
        play(pdata, TRUE);
    }
}
int ns2s(gint64 ns){
    return (ns/1000/1000/1000);
}

char *play_query_duration(gst_data *pdata, char *buf) {
	gint64 dur;
	gst_element_query_duration(pdata->playbin, GST_FORMAT_TIME, &dur);
    sprintf(buf,"%d",  ns2s(dur));
    return buf;
}


char *play_query_status(gst_data *pdata, char *buf) {
    GstState state;
    gst_element_get_state(pdata->playbin, &state, NULL,GST_CLOCK_TIME_NONE);
    if (state == GST_STATE_PLAYING) {
        sprintf(buf, "%s", "play");
    }else{
        sprintf(buf, "%s", "pause");
    }
    return buf;
}

char *play_query_file_names(gst_data *pdata, int min,int max, char *buf) {
    play_list_get_file_names(pdata->play_list, min, max, buf);
    return buf;
}


char *play_query_abulum(gst_data *pdata, char *buf) {
    play_list_get_album(pdata->play_list, buf);
    return buf;
}

char *play_query_title(gst_data *pdata, char *buf){
    play_list_get_title(pdata->play_list, buf);
    return buf;
}

char *play_query_singer(gst_data *pdata, char *buf) {
    play_list_get_artist(pdata->play_list, buf);
    return buf;
}

char * play_query_totals_songs(gst_data *pdata, char *buf){
    play_list_get_totals_songs(pdata->play_list, buf);
    return buf;
}

char * play_query_cur_offset(gst_data *pdata, char *buf){
    play_list_get_cur_offset(pdata->play_list, buf);
    return buf;
}

char *play_query_position(gst_data *pdata, char *buf) {
	gint64 off;
	gst_element_query_position(pdata->playbin, GST_FORMAT_TIME, &off);
    sprintf(buf,"%d",  ns2s(off));
    return buf;
}


void player_play(gst_data *pdata) {
    switch (pdata->state) {
        case cust_pause: {
            g_print(" cust_pause  \n");
            pdata->state = cust_play;
            GstElement *playbin = pdata->playbin;
            gst_element_set_state(playbin, GST_STATE_PLAYING);
        }
        case cust_list_have: {
            g_print(" cust_list_have  \n");
            pdata->state = cust_play;
            player_next(pdata);
            break;
        }
        case cust_list_empty:
        case cust_play:
            break;
    }
}


void player_pause(gst_data *pdata) {
    GstElement *playbin = pdata->playbin;
    if (pdata->state == cust_play) {
        pdata->state = cust_pause;
    }
    gst_element_set_state(playbin, GST_STATE_PAUSED);
}

static void gst_free(gst_data *pdata) {
    GstElement *playbin = pdata->playbin;
    gst_element_set_state(playbin, GST_STATE_NULL);
    gst_object_unref(playbin);
    g_source_remove(pdata->bus_watch);
    g_main_loop_unref(pdata->loop);
    play_list_destroy(&pdata->play_list);
    usb_monitor_destor(&pdata->usb_monitor);
}

void player_list_add(gst_data *pdata, const char *dir_name) {
    music_scan(pdata, dir_name);
}

void player_list_remove(gst_data *pdata, const char *dir_name) {
    music_song_remove(pdata, dir_name);
}

static void music_song_remove(gst_data *pdata, const gchar *dir_name) {
    char *dir = gst_filename_to_uri(dir_name, NULL);
    play_list_remove_by_name(pdata->play_list, dir);
    g_free(dir);
    // 判断为空列表时， 停住并且释放播放信息
    if (play_list_is_empty(pdata->play_list)) {
        pdata->state = cust_list_empty;
        GstElement *playbin = pdata->playbin;
        gst_element_set_state(playbin, GST_STATE_READY);
        gst_element_set_state(playbin, GST_STATE_NULL);
    }


}

static void music_scan(gst_data *pdata, const gchar *dir_name) {
    scan_dir(dir_name, pdata->play_list);
    if (play_list_is_empty(pdata->play_list)) {
        pdata->state = cust_list_empty;
    } else {
        if (pdata->state == cust_list_empty) {
            pdata->state = cust_list_have;
        }
    }
}

static void music_init(gst_data *data) {
    memset(data, 0, sizeof(gst_data));
    data->play_list = play_list_new();
    data->usb_monitor = usb_monitor_new();
    data->state = cust_list_empty;
}

#if 1

void *add_function(void *data) {
    gst_data *pdata = (gst_data *) data;
    for (int i = 0; i < 2; i++) {
        sleep(5);
        g_print("----------------------add-------------------------\n");
        player_list_add(pdata, "/home/bingo/music2");
        sleep(15);
        g_print("----------------------remove-----------------------\n");
        player_list_remove(pdata, "/home/bingo/music2");
    }
    return NULL;
}

#endif

int main(int argc, char *argv[]) {
    gst_data data;
    pthread_t id = 0;
    gst_init(NULL, NULL);
    printf("Locale is: %s\n", setlocale(LC_ALL, "zh_CN.utf8"));
    music_init(&data);
//    player_list_add(&data, "/home/bingo/music1");
    new_player(&data);
    data.loop = g_main_loop_new(NULL, FALSE);
//    pthread_create(&id, NULL, add_function, &data);
//    if (err != 0) {
//        printf("cont creat the pthread \n");
//    }
    usb_monitior_blocked_notice(data.usb_monitor, &data);
    player_play(&data);
    g_main_loop_run(data.loop);
    gst_free(&data);
    pthread_join(id, NULL);
    return 0;
}