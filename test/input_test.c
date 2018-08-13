//
// Created by bingo on 18-8-6.
//

#include <stdio.h>
#include <unistd.h>
#include <linux/input-event-codes.h>
#include <linux/input.h>


int main()
{
    struct  input_event event;
    int fd = STDIN_FILENO;
    int ret = 0;

   while ((ret = read(fd, &event, sizeof(event))) > 0) {
        if(event.code == KEY_PREVIOUSSONG)
            printf(" %d   %d  %d  \n", event.type, event.code, event.value);
        if(event.code == KEY_PLAYPAUSE)
            printf(" %d   %d  %d  \n", event.type, event.code, event.value);
        if(event.code == KEY_VOLUMEUP){
            printf(" %d   %d  %d  \n", event.type, event.code, event.value);
        }
        if(event.code == KEY_VOLUMEDOWN){
            printf(" %d   %d  %d  \n", event.type, event.code, event.value);
        }
        if(event.code == KEY_NEXTSONG){
            printf(" %d   %d  %d  \n", event.type, event.code, event.value);
        }
    }
    if (ret == -1) {
        perror(" ");
    }
    return 0;
}
