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
#include <player_opt.h>
#include "play_list.h"
#include "player.h"
#include "socket_observer.h"

#define BLOCK_ID_INVALID  ( -1 )

static void music_scan(gst_data_t *pdata, const gchar *dir_name);

static void music_song_remove(gst_data_t *pdata, const gchar *dir_name);

static void player_list_add(gst_data_t *pdata, const char *dir_name);

static void player_list_remove(gst_data_t *pdata, const char *dir_name);

static void player_next(gst_data_t *pdata);

static void player_prev(gst_data_t *pdata);

static void player_play(gst_data_t *pdata);

static void player_pause(gst_data_t *pdata);

static int ns2s(gint64 ns);

gchar music[][8] =
        {
                {".flac"},
                {".mp3"},
                {".wav"},
                {".ogg"},
                {".dsf"},
        };

static gboolean music_compare(const gchar *name) {
    int i;
    int sz = sizeof(music) / sizeof(music[0]);
    gint result = 0;
    for (i = 0; i < sz; i++) {
        if (!(result = strcmp(name, music[i]))) {
            return TRUE;
        }
    }
    return FALSE;
}

static void scan_dir(const gchar *dir_name, play_list_t *play_list) {
    GError *err = NULL;
    GDir *dir;
    const gchar *entry;
    GList *index, *dirList = NULL;
	gchar *path = NULL;
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
        path = g_build_filename(dir_name, entry, NULL);
        if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
            dirList = g_list_append(dirList, path);
        } else if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
            const gchar *c = entry;
            while (*c != '\0') {
                if (*c == '.' && music_compare(c)) {
                    gchar *ch = gst_filename_to_uri(path, NULL);
                    g_print("----ADD list----   %s \n", ch);
                    play_list_add_song(play_list, ch, NULL, NULL, NULL) ;
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

static void analyze_streams(gst_data_t *data) {
    GstTagList *tags = NULL;
    gchar *str;
	play_list_t * play_list = data->play_list;
	player_opt_t * opt = player_get_opt(data);
	gint n_video, n_audio, n_text;
	g_object_get (data->playbin, "n-video", &n_video, NULL);
	g_object_get (data->playbin, "n-audio", &n_audio, NULL);
	g_object_get (data->playbin, "n-text", &n_text, NULL);

	g_print ("%d video stream(s), %d audio stream(s), %d text stream(s)\n",
    n_video, n_audio, n_text);

  	g_print ("\n");
  	for(int i = 0; i < n_audio; i ++){
  	    g_signal_emit_by_name(data->playbin, "get-audio-tags", i, &tags);
	    if (tags) {
	        g_print("audio stream %d:\n", i);
	        if (gst_tag_list_get_string(tags, GST_TAG_ARTIST, &str)) {
	            g_print("  ARTIST: %s\n", str);
	            play_list_set_artist(play_list, str);
				player_opt_event_add(opt, mcu_info_singer_str);
	        }
	        if (gst_tag_list_get_string(tags, GST_TAG_ALBUM, &str)) {
	            g_print("  ALBUM: %s\n", str);
	            play_list_set_album(play_list, str);
				player_opt_event_add(opt, mcu_info_ablume_str);
			}
			 if (gst_tag_list_get_string (tags, GST_TAG_LANGUAGE_CODE, &str)) {
			    g_print ("  language: %s\n", str);
			    g_free (str);
			  }
			gst_tag_list_free (tags);
	    }
  	}

   
}

static gboolean play_bus_msg(GstBus *bus, GstMessage *msg, gpointer user_data) {
    GError *err;
    gchar *debug_info;
    gst_data_t *pdata = (gst_data_t *) user_data;
	player_opt_t * opt = player_get_opt( pdata);
    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element : %s\n", err->message);
            g_printerr("Debugging information: %s\n",
                       debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            player_opt_set_cmd(opt, remove_player_list);
            break;
        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            play_list_clear_titil_album_artist(pdata->play_list);
            player_opt_set_cmd(opt, usr_next);
            break;
        case GST_MESSAGE_STATE_CHANGED: {
            GstState old_state, new_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state,NULL);
            if (GST_MESSAGE_SRC (msg) == GST_OBJECT (pdata->playbin)) {
           		if (GST_STATE_PLAYING == new_state) {
           			g_printf("GST_MESSAGE_SRC \n");
           			analyze_streams (pdata);
           		}
            }
            break;
        }
        default:
        //g_printerr("Unexpected message received.\n");
        break;
    }
    return TRUE;
}

gboolean new_player(gst_data_t *pdata) {
    GstElement *playbin = NULL;
    playbin = gst_element_factory_make("playbin", "audio");
    if (playbin == NULL) {
        g_printerr("failure to malloc the playbin");
        return FALSE;
    }
    pdata->bus = gst_element_get_bus(playbin);
    gst_bus_add_watch(pdata->bus, (GstBusFunc)play_bus_msg, pdata);
    pdata->playbin = playbin;
    return TRUE;
}

static void play_uri(GstElement *playbin, play_list_t *play_list, gboolean isNext) {
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
        gst_element_set_state(playbin, GST_STATE_PLAYING);
		//gst_element_get_state (playbin, NULL, NULL, -1);

    } 
}

static void play(gst_data_t *pdata, gboolean isNext) {
    GstElement *playbin = pdata->playbin;
    play_uri(playbin, pdata->play_list, isNext);
}

void player_prev(gst_data_t *pdata) {
        play(pdata, FALSE);
}

void player_next(gst_data_t *pdata) {
        play(pdata, TRUE);
}

int ns2s(gint64 ns){
    return (ns/1000/1000/1000);
}

char *player_query_duration(gst_data_t *pdata, char *buf) {
	gint64 dur =0;
	gboolean ret = FALSE;
	do{
		ret = gst_element_query_duration(pdata->playbin, GST_FORMAT_TIME, &dur);
	}while(ret != TRUE);
	sprintf(buf,"%d",  ns2s(dur));
    return buf;
}


char *player_query_status(gst_data_t *pdata, char *buf) {
    GstState state,pend = -1;
    while( !(GST_STATE_CHANGE_SUCCESS ==gst_element_get_state(pdata->playbin, &state,  &pend,10*GST_SECOND)&&
    pend == GST_STATE_VOID_PENDING)){
    	;
    }
   // state = GST_STATE();
    if (state == GST_STATE_PLAYING) {
    	sprintf(buf, "%s", "play");
    }else{
        sprintf(buf, "%s", "pause");
    }
    return buf;
}

int player_query_file_names(gst_data_t *pdata, int min,int max, char *buf, const char *prefix) {
    return play_list_get_file_names(pdata->play_list, min, max, buf, prefix);
}


char *player_query_abulum(gst_data_t *pdata, char *buf) {
    play_list_get_album(pdata->play_list, buf);
    return buf;
}

char *player_query_title(gst_data_t *pdata, char *buf){
    play_list_get_title(pdata->play_list, buf);
    return buf;
}

char *player_query_singer(gst_data_t *pdata, char *buf) {
    play_list_get_artist(pdata->play_list, buf);
    return buf;
}

char * player_query_totals_songs(gst_data_t *pdata, char *buf){
    play_list_get_totals_songs(pdata->play_list, buf);
    return buf;
}

char * player_query_cur_offset(gst_data_t *pdata, char *buf){
    play_list_get_cur_offset(pdata->play_list, buf);
    return buf;
}

char *player_query_position(gst_data_t *pdata, char *buf) {
	gint64 off;
	gst_element_query_position(pdata->playbin, GST_FORMAT_TIME, &off);
    sprintf(buf,"%d",  ns2s(off));
    return buf;
}

void player_play(gst_data_t *pdata) {
	gchar * pname;
	GstElement *playbin =  pdata->playbin;
	g_object_get(playbin, "uri", &pname, NULL);	
	if(pname == NULL){
		player_next(pdata);
		return;
	} 
	gst_element_set_state(playbin, GST_STATE_PLAYING);
	g_free(pname);
}

player_opt_t * player_get_opt(gst_data_t *pdata){
	player_opt_t * opt = &pdata->player_opt;
	return opt;
}

void player_pause(gst_data_t *pdata) {	
	GstElement * playbin =  pdata->playbin;
    gst_element_set_state(playbin, GST_STATE_PAUSED);
}

void player_destory(gst_data_t *pdata) {
    GstElement *playbin = pdata->playbin;
     gst_object_unref(pdata->bus);
    gst_element_set_state(playbin, GST_STATE_NULL);
    gst_object_unref(playbin);
  //  g_main_loop_unref(pdata->loop);
    play_list_destroy(&pdata->play_list);
    usb_monitor_destor(&pdata->usb_monitor);
}

void player_list_add(gst_data_t *pdata, const char *dir_name) {
    music_scan(pdata, dir_name);
}

void player_list_remove(gst_data_t *pdata, const char *dir_name) {
    music_song_remove(pdata, dir_name);
}

static void music_song_remove(gst_data_t *pdata, const gchar *dir_name) {
	GstElement *playbin = NULL;
    char *dir = gst_filename_to_uri(dir_name, NULL);
    play_list_remove_by_name(pdata->play_list, dir);
    g_free(dir);
    // 判断为空列表时， 停住并且释放播放信息
    if (play_list_is_empty(pdata->play_list)) {
        playbin = pdata->playbin;
        gst_element_set_state(playbin, GST_STATE_READY);
        gst_element_set_state(playbin, GST_STATE_NULL);
    }


}

static void music_scan(gst_data_t *pdata, const gchar *dir_name) {
    scan_dir(dir_name, pdata->play_list);
}

void player_init(gst_data_t *data) {
    memset(data, 0, sizeof(gst_data_t));
    data->play_list = play_list_new();
    data->usb_monitor = usb_monitor_new();
    player_opt_init(&data->player_opt);
}

gboolean player_handle_cmd(gst_data_t * data){
    int cmd = 0;
    char buf[1024];
    player_opt_get_cmd(&data->player_opt,&cmd,buf);
    player_opt_t * popt = player_get_opt (data);
    switch (cmd){
        case add_player_list:
            player_list_add(data, buf);
            if(!play_list_is_empty(data->play_list)){
             	player_opt_set_state(popt, state_have);
            }
            break;
        case remove_player_list:
            player_list_remove(data, buf);
            if(play_list_is_empty(data->play_list)){
             	player_opt_set_cmd(popt, state_empty);
            }
            break;
        case usr_play:
            player_play(data);
            player_opt_set_state(popt, state_play);
            break;
        case usr_pause:
            player_pause(data);
            player_opt_set_state(popt, state_play);
            break;
        case usr_next:
            player_next(data);
            player_opt_set_state(popt, state_play);
            break;
        case usr_previours:
            player_prev(data);
            player_opt_set_state(popt, state_play);
            break;
        default:
            break;
    }
    return TRUE;

}

void player_runing(gst_data_t *data){
    int is_continue = TRUE;
    while(is_continue){
        player_opt_main_loop(&data->player_opt);
        is_continue = player_handle_cmd(data);
    }
}


