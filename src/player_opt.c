//
// Created by bingo on 8/19/18.
//

#include <stdbool.h>
#include <stdio.h>
#include "player_opt.h"

#define BUF_SZ 20
#define ADD_EVENT_BY_TYPE(ARR) \
{  \
    for(int i = 0; i < ARRSIZE(ARR); i ++){\
        p_opt->events = g_list_append(p_opt->events, (void *)ARR[i]);\
    }\
}


char * state_empty_message[BUF_SZ] = {
    mcu_info_play_pause_str,
    NULL,
};

char * state_pause_message[BUF_SZ] = {
    mcu_info_play_pause_str,
    mcu_info_index_str,
    NULL,
};

char * state_play_message[BUF_SZ] = {
    mcu_info_play_pause_str,
    mcu_info_name_str,
    mcu_info_index_str,
    mcu_info_play_time_str,
    mcu_info_play_total_time_str,
    NULL,
};

char * state_have_message[BUF_SZ] = {
	mcu_cmd_next_str,
    NULL,
};

char * fail_set_on_empty[BUF_SZ] = {
    mcu_info_play_pause_str,
    NULL,
};

void player_opt_init(player_opt_t *p_opt) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&p_opt->mutex, &attr);
    pthread_cond_init(&p_opt->cond, NULL);
    p_opt->events = NULL;
    p_opt->status = state_empty;
    p_opt->cmd = 0;
}

// must free outside
char *player_opt_event_pop_font(player_opt_t *p_opt) {
    gchar *ret = NULL;
    pthread_mutex_lock(&p_opt->mutex);
    if (p_opt->events) {
        ret = (char *) p_opt->events->data;
        p_opt->events = g_list_delete_link(p_opt->events, p_opt->events);
    }
    pthread_mutex_unlock(&p_opt->mutex);
    return ret;
}

void player_opt_destroy(player_opt_t *p_opt) {
    GList *tmp, *itr = p_opt->events;
    while (itr != NULL) {
        tmp = itr;
        itr = itr->next;
        free(tmp->data);
        free(tmp);
    }
    p_opt->events = NULL;
    p_opt->status = state_empty;
    pthread_mutex_destroy(&p_opt->mutex);
    pthread_cond_destroy(&p_opt->cond);
}

//void player_opt_clear_cmd(player_opt_t *p_opt) {
//	pthread_mutex_lock(&p_opt->mutex);
//	if(p_opt->cmd_args){
//		g_free(p_opt->cmd_args);
//		p_opt->cmd_args = NULL;
//	}
//    p_opt->cmd = cmd_init;
//	pthread_mutex_unlock(&p_opt->mutex);
//}

void player_opt_set_cmd_with_buf(player_opt_t *p_opt, int cmd, char * cmd_args) {
	pthread_mutex_lock(&p_opt->mutex);
	strcpy(p_opt->cmd_args, cmd_args);
	pthread_mutex_unlock(&p_opt->mutex);
	player_opt_set_cmd(p_opt, cmd);
}

void player_opt_set_cmd(player_opt_t *p_opt, int cmd) {
    pthread_mutex_lock(&p_opt->mutex);
    switch (cmd) {
        case usr_play:
        case usr_pause:
        case usr_next:
        case usr_previours:
            if (p_opt->status != state_empty) {
                p_opt->cmd = cmd;
            } else {
                //todo
                ADD_EVENT_BY_TYPE(fail_set_on_empty);
                p_opt->cmd = cmd;
                goto out_without_nodity;
            }
       goto out;
    }
    if (cmd == add_player_list || cmd == remove_player_list) {
        p_opt->cmd = cmd;
    }
    out:
    pthread_cond_signal(&p_opt->cond);
    out_without_nodity:
    pthread_mutex_unlock(&p_opt->mutex);

}

void player_event_add(player_opt_t *p_opt, const char *event) {
    pthread_mutex_lock(&p_opt->mutex);
    p_opt->events = g_list_append(p_opt->events, (void *)event);
    pthread_mutex_unlock(&p_opt->mutex);
}

void player_event_arrays_add(player_opt_t *p_opt,  char *idx[20]) {
	char * pch = idx[0];
 	pthread_mutex_lock(&p_opt->mutex);
	do{
		p_opt->events = g_list_append(p_opt->events, pch);
		pch = (++idx)[0];
	}while(pch);
	pthread_mutex_unlock(&p_opt->mutex);
}

int player_opt_get_cmd(player_opt_t *p_opt, int *cmd, char *buf) {
    pthread_mutex_lock(&p_opt->mutex);
    *cmd = p_opt->cmd;
    strcpy(buf, p_opt->cmd_args);
    pthread_mutex_unlock(&p_opt->mutex);
    return (int) (strlen(buf) + 1);
}

void player_opt_main_loop(player_opt_t *p_opt) {
    pthread_mutex_lock(&p_opt->mutex);
    pthread_cond_wait(&p_opt->cond, &p_opt->mutex);
    pthread_mutex_unlock(&p_opt->mutex);
}

void player_opt_set_state(player_opt_t *p_opt, int state) {
	pthread_mutex_lock(&p_opt->mutex);
	switch(state){
	case state_empty:
		player_event_arrays_add(p_opt, state_empty_message);
		break;
    case state_have:
	    player_event_arrays_add(p_opt, state_have_message);
	    break;
    case state_play:
	    player_event_arrays_add(p_opt, state_play_message);
	    break;
    case state_pause:
	    player_event_arrays_add(p_opt, state_pause_message);
	    break;
	}
	p_opt->status = state;
	pthread_mutex_unlock(&p_opt->mutex);
}

void player_opt_event_add(player_opt_t *p_opt, char *event) {
	pthread_mutex_lock(&p_opt->mutex);
    p_opt->events = g_list_append(p_opt->events, event);
    pthread_mutex_unlock(&p_opt->mutex);
}