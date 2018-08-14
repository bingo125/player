//
// Created by bingo on 18-7-31.
//

#include <memory.h>
#include <stdio.h>
#include "net_impl.h"
#include "player.h"
#include "protocol.h"
#include "usb_scan.h"
static GList ** _write_buf = NULL;
char *mount_cb(callback_t *cb, char *cmd, gpointer userdata) {

	g_print("mount:%s\n", cmd);
	player_list_add(userdata, cmd);
	player_next_cb(cb, userdata, cmd);
	return NULL;
}

char *unmount_cb(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("unmount_cb:%s\n", cmd);
	player_list_remove(userdata, cmd);
	return NULL;
}

char *player_next_cb(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("player_next_cb:\n");
	player_next(userdata);
	return NULL;
}

char *player_prev_cb(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("player_prev_cb:\n");
	player_prev(userdata);
	return NULL;
}

char *player_play_cb(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("player_play_cb:\n");
	player_play(userdata);
	return NULL;
}

char *player_pause_cb(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("player_pause_cb:\n");
	player_pause(userdata);
	return NULL;
}

char *play_info_pause(callback_t *cb, char *cmd, gpointer userdata) {
	g_print("play_info_pause:\n");
	return strdup("player info plause");
}

#define BUF_SZ 128

char *player_info_cur_cb(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_position(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_info_duration_cb(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_duration(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_feedback_status(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_status(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

#include "player.h"
char *player_info_songs_name(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];

	gint min , max;
	min = max = -1;
	GRegex* regex;
	GMatchInfo *match_info;
	regex = g_regex_new("[0-9]+", 0 , 0, NULL);
	g_regex_match(regex, cmd, 0, &match_info);
	while (g_match_info_matches(match_info)) {
		gchar* word = g_match_info_fetch(match_info, 0);
		g_print("%s\n",word);
		if (min == -1) {
			min = atoi(word);
		}else{
			max = atoi(word);
		}
		g_free(word);
		g_match_info_next(match_info, NULL);
	}
	g_match_info_free(match_info);
	g_regex_unref(regex);
	play_query_file_names(userdata, min, max, buf);

	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);

}


char *player_info_abulum(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_abulum(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_info_singer(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_singer(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_info_title(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_title(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_info_cur_offset(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_cur_offset(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

char *player_info_totals_songs(callback_t *cb, char *cmd, gpointer userdata) {
	char buf[BUF_SZ];
	char ret[BUF_SZ];
	play_query_totals_songs(userdata, buf);
	sprintf(ret, "%s=%s", cb->freeback, buf);
	return strdup(ret);
}

#define MOUNT "VOLUME MOUNT="
#define UNMOUNT "VOLUME UNMOUNT="

callback_t callbacks[] = {
	{
		.match_id =MOUNT,
		.freeback = NULL,
		.function = mount_cb,
	},
	{
		.match_id = UNMOUNT,
		.function = unmount_cb,
		.freeback = NULL,
	},
	{
		.match_id = mcu_cmd_next_str,
		.function = player_next_cb,
		.freeback = NULL,
	},
	{
		.match_id = mcu_cmd_pre_str,
		.function = player_prev_cb,
		.freeback = NULL,
	},
	{
		.match_id = mcu_cmd_play_str,
		.function = player_play_cb,
		.freeback = NULL,
	},
	{
		.match_id = mcu_cmd_pause_str,
		.function = player_pause_cb,
		.freeback = NULL,
	},

	{
		.match_id = mcu_info_play_pause_str,
		.freeback = arm_feedback_play_pause_str,
		.function = player_feedback_status,

	},
	{
		.match_id = mcu_info_loop_str,
		.freeback = arm_feedback_loop_str,
	},
	{
		.match_id = mcu_info_play_time_str,
		.freeback = arm_feedback_time_play_str,
		.function = player_info_cur_cb,

	},
	{
		.match_id = mcu_info_play_total_time_str,
		.freeback = arm_feedback_time_song_str,
		.function = player_info_duration_cb,
	},
	{
		.match_id = mcu_info_index_str,
		.freeback = arm_feedback_cur_str,
		.function = player_info_cur_offset,
	},
	{
		.match_id = mcu_info_total_str,
		.freeback = arm_feedback_total_song_str,
		.function = player_info_totals_songs,
	},
	{
		.match_id = mcu_info_name_str,
		.freeback = arm_feedback_name_str,
		.function = player_info_title,
	},
	{
		.match_id = mcu_info_singer_str,
		.freeback = arm_feedback_singer_str,
		.function = player_info_singer,
	},
	{
		.match_id = mcu_info_ablume_str,
		.freeback = arm_feedback_ablum_str,
		.function = player_info_abulum,
	},
	{
		.match_id = mcu_info_file_name_str,
		.freeback = arm_feedback_file_names_str,
		.function = player_info_songs_name,
		.muti_char= TRUE,
	},
	{
		.match_id = NULL,
		.function = NULL,
	},
};

//todo 后续此处使用二叉搜索，加快搜索速度
callback_t *net_impl_cbs() {
	return callbacks;
}


void net_impl_set_buf(GList** buf){
	_write_buf = buf;
}