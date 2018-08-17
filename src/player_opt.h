//
// Created by bingo on 8/17/18.
//

#ifndef PLAYER_PLAYER_OPT_H
#define PLAYER_PLAYER_OPT_H

#include <glib.h>
typedef enum {
    list_empty,
    play,
    pause,
}player_status_t;


enum {
    add_player_list,
    remove_player_list,
    usr_play,
    usr_pause,
    next,
    previours,
};


typedef struct {
    pthread_rwlock_t rwlock;
    player_status_t status;
    gboolean  has_change;
    GSList * events;
}player_opt_t;


void player_opt_init(player_opt_t * p_opt);

void player_event_push_back(player_opt_t *p_opt);

// must free outside
char *player_event_pop_font(player_opt_t *p_opt);

void player_opt_destroy(player_opt_t * p_opt);

#endif //PLAYER_PLAYER_OPT_H
