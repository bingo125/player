//
// Created by bingo on 18-7-31.
//

#include <memory.h>
#include "net_impl.h"
#include "player.h"
#include "protocol.h"

char *mount_cb(gpointer userdata, char *mount) {

	g_print("mount:%s\n", mount);
	player_list_add(userdata, mount);
	player_next_cb(userdata, mount);
	return NULL;
}

char *unmount_cb(gpointer userdata, char *mount) {
	g_print("unmount_cb:%s\n", mount);
	player_list_remove(userdata, mount);
	return NULL;
}

char *player_next_cb(gpointer userdata, char *cmd) {
	g_print("player_next_cb:\n");
	player_next(userdata);
	return NULL;
}

char *player_prev_cb(gpointer userdata, char *cmd) {
	g_print("player_prev_cb:\n");
	player_prev(userdata);
	return NULL;
}

char *player_play_cb(gpointer userdata, char *cmd) {
	g_print("player_play_cb:\n");
	player_play(userdata);
	return NULL;
}

char *player_pause_cb(gpointer userdata, char *cmd) {
	g_print("player_pause_cb:\n");
	player_pause(userdata);
	return NULL;
}


char *play_info_pause(gpointer userdata, char *cmd) {
	g_print("play_info_pause:\n");
	return strdup("player info plause");
}

#define MOUNT "VOLUME MOUNT="
#define UNMOUNT "VOLUME UNMOUNT="
#define NEXT "PLAYER NEXT"
#define PREV "PLAYER PREV"
#define PLAY "PLAYER PLAY"
#define PAUSE "PLAYER PAUSE"

callback_t callbacks[] = {
	{
		.match_id =MOUNT,
		.function = mount_cb,
	},
	{
		.match_id = UNMOUNT,
		.function = unmount_cb,
	},
	{
		.match_id = mcu_cmd_next_str,
		.function = player_next_cb,
	},
	{
		.match_id = mcu_cmd_pre_str,
		.function = player_prev_cb,
	},
	{
		.match_id = mcu_cmd_play_str,
		.function = player_play_cb,
	},
	{
		.match_id = mcu_cmd_pause_str,
		.function = player_pause_cb,
	},
	{
		.match_id = mcu_info_play_pause_str,
		.function = play_info_pause,
	},
	{
		.match_id = NULL,
		.function = NULL,
	},
};
//todo 后续此处使用二叉搜索，加快搜索速度
const callback_t *net_impl_cbs() {
	return callbacks;
}
