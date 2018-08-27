//
// Created by bingo on 8/17/18.
//

#ifndef PLAYER_PLAYER_OPT_H
#define PLAYER_PLAYER_OPT_H

#include <glib.h>
#include "protocol.h"

typedef enum {
    stata_init = 0x55,
    state_empty,
    state_have,
    state_play,
    state_pause,

    cmd_init = 0xaa,
    add_player_list,
    remove_player_list,
    usr_play,
    usr_pause,
    usr_next,
    usr_previours,
    usr_play_toggle,
} player_status_t;

enum event_idx {
    event_empty_to_full,
    event_full_to_play,
    event_full_to_pause,
    event_pause_to_empty,
    event_play_to_empty,
    event_play_to_play,
    event_fail_set_on_empty,
};



#define  BFSZ 1024
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    player_status_t status;
    int cmd;
    char cmd_args[BFSZ];
    gboolean has_change;
    GList *events;
} player_opt_t;


void player_opt_init(player_opt_t *p_opt);

char *player_opt_event_pop_font(player_opt_t *p_opt);

void player_opt_destroy(player_opt_t *p_opt);

int player_opt_get_cmd(player_opt_t *p_opt, int *cmd, char *buf);

void player_opt_main_loop(player_opt_t *p_opt);

void player_opt_event_add(player_opt_t *p_opt, char *event) ;

void player_opt_set_cmd(player_opt_t *p_opt, int cmd);

void player_opt_set_cmd_with_buf(player_opt_t *p_opt, int cmd, char * cmd_args) ;

void player_opt_set_state(player_opt_t *p_opt, int state) ;



#endif //PLAYER_PLAYER_OPT_H

