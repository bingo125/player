//
// Created by bingo on 8/20/18.
//



#include <glib.h>
#include "player_opt.h"

void player_opt_setup(player_opt_t * player_opt, gconstpointer test){
    g_print("player_opt_setup\n");

}

void player_opt_teardonw(player_opt_t * player_opt, gconstpointer test){

    g_print("player_opt_teardonw\n");
}
void player_test(player_opt_t * player_opt, gconstpointer test){
    g_assert_cmpint(1, ==, 3);
}

int main(int argc, char *argv[]){
    g_test_init(&argc, &argv, NULL);
    g_test_add("/player/player_opt", player_opt_t, NULL,player_opt_setup, player_test,player_opt_teardonw);
    return g_test_run();
}