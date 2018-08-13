#include "player_input.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>



int input_init(input_t * input_dev, const char * dev_path){
   int fd = open(dev_path, O_RDONLY);
   if(fd == -1){
       return -1;
   }
   input_dev ->fd = fd;
}


gboolean match(input_t * input_dev,gpointer userdata, char *mount){
    struct input_event * event = (struct input_event*)mount;
    if (event.type == EV_KEY && event.value == 0){
        switch(event.code){
            case KEY_NEXTSONG:
            case KEY_PREVIOUSSONG:
            case KEY_PLAYPAUSE:
            case KEY_VOLUMEUP:
            case KEY_VOLUMEDOWN:
                return TRUE;
        }
    }
    return FALSE;
}
int input_destory(input_t * input_dev){
    close(input_dev->fd);
}
