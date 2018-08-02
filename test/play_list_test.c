//
// Created by bingo on 18-7-28.
//

#include "play_list.h"
#include <pthread.h>
#include <unistd.h>

int flag = 0;
void * pthread_runting(void *userdata)
{
    play_list_t * t = (play_list_t*) userdata;
    sleep(1);
    play_list_add_song(t, "file:///music1/1",NULL,NULL,NULL);
    play_list_add_song(t, "file:///music1/2",NULL,NULL,NULL);
    play_list_add_song(t, "file:///music1/3",NULL,NULL,NULL);
    for(int i = 0 ; i < 3; i++){
        sleep(10);
        g_print("-------------------------------------------remove acttion ---------------------------------- \n");
        play_list_remove_by_name(t, "file:///music1");
        flag = 1;
        sleep(6);
        g_print("############################################add acttion ############################################\n");
        play_list_add_song(t, "file:///music1/1",NULL,NULL,NULL);
        play_list_add_song(t, "file:///music1/2",NULL,NULL,NULL);
        play_list_add_song(t, "file:///music1/3",NULL,NULL,NULL);

    }
    return NULL;
}
int main()
{
    pthread_t id;
    play_list_t *t = play_list_new();
    pthread_create(&id, NULL, pthread_runting,t);
    for(int i = 0 ; i < 200; )
    {
        gchar * pch = NULL;
        if(flag){
            printf("----\n");
            flag = !flag;

        }
        if((pch = play_list_next(t)) != NULL){
            i  ++;

            g_print("%d: %s   \n", i, pch);
        }
        sleep(1);

    }
    play_list_destroy(&t);
    pthread_join(&id,NULL);
    return 0;
}