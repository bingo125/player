//
// Created by bingo on 8/20/18.
//

#include <stdio.h>
#include <player.h>
#include <gst/gst.h>
#include <locale.h>
#include "player.h"
#include "socket_observer.h"

void * pthread_ruting(void * pdata){
	gst_data_t * pgst = pdata;
	pgst->loop = g_main_loop_new(NULL, FALSE);
	usb_monitior_blocked_notice(pgst->usb_monitor, pgst);
	g_main_loop_run(pgst->loop);
	return NULL;
}
int main(int argc, char *argv[]) {
    gst_data_t data;
    pthread_t pid;
    gst_init(NULL, NULL);
    printf("Locale is: %s\n", setlocale(LC_ALL, "zh_CN.utf8"));
    player_init(&data);
    new_player(&data);
    pthread_create(&pid, NULL,pthread_ruting, &data);
    usb_monitior_blocked_notice(data.usb_monitor, &data);
    player_runing(&data);
    pthread_cancel(pid);
	player_destory(&data);
    return 0;
}
