//
// Created by bingo on 18-8-6.
//

#ifndef GST_PLAY_PLAYER_INPUT_H
#define GST_PLAY_PLAYER_INPUT_H


#include <glib.h>

typedef struct {
    int fd;
}input_t;

int input_init(input_t * input_dev, const char * dev_path);
int input_init_get_fd(input_t * input_dev);
void match(input_t * input_dev,gpointer userdata, char *mount);
int input_destory(input_t * input_dev);

#endif //GST_PLAY_PLAYER_INPUT_H
