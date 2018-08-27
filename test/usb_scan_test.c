//
// Created by bingo on 18-7-28.
//

//#include "usb_scan.h"
//
//#include <sys/inotify.h>
//
//int main() {
////    printf(" hello wrold \n");
////
////    socket_observer_t * p_usb = usb_monitor_new();
////    usb_monitior_blocked_notice(p_usb, NULL, NULL,NULL);
//    return 0;
//}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/unistd.h>
#include <linux/inotify.h>
//#include <glib/gtypes.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>


char *event_array[] = {
        "File was accessed",
        "File was modified",
        "File attributes were changed",
        "writtable file closed",
        "Unwrittable file closed",
        "File was opened",
        "File was moved from X",
        "File was moved to Y",
        "Subfile was created",
        "Subfile was deleted",
        "Self was deleted",
        "Self was moved",
        "",
        "Backing fs was unmounted",
        "Event queued overflowed",
        "File was ignored"
};
#define EVENT_NUM 16
#define MAX_BUF_SIZE 1024

int quit = 0;

void *pthread_runting(void *data) {
    char *path = (char *) data;
    DIR *dir = NULL;
    sleep(1);
    while ((dir = opendir(path)) == NULL) {
        printf(" dir === NULL \n");
    }
    struct dirent *dirent;
    while ((dirent = readdir(dir)) != NULL) {
        if (quit) {
            break;
        }
        sleep(1);
        printf("------%s-------\n", dirent->d_name);
    }
    printf("------%s out -------\n", path);
    closedir(dir);
    return NULL;
}

int main(int argc, char *argv[]) {
    int fd, wd;
    char buffer[MAX_BUF_SIZE + 1];
    char *offset = NULL;
    struct inotify_event *event;
    int i, len, tmp_len;
    char strbuf[16];
    pthread_t id;
    if ((fd = inotify_init()) < 0) {
        printf("Fail to initialize inotify.\n");
        exit(0);
    }
    if ((wd = inotify_add_watch(fd, "/run/media", IN_CREATE | IN_DELETE)) < 0) {
        printf("Can't add watch for %s.\n", argv[1]);
        exit(0);
    }
    while (len = read(fd, buffer, MAX_BUF_SIZE)) {
        offset = buffer;
        event = (struct inotify_event *) buffer;
        while (((char *) event - buffer) < len) {
            printf("Object type: %s\n",
                   event->mask & IN_ISDIR ? "Direcotory" : "File");
            if (event->wd != wd)
                continue;
            printf("Object name: %s\n", event->name);
            printf("Event mask: %08X\n", event->mask);
            printf("Event mask: %08X\n");

            if (event->mask & IN_DELETE) {
                quit = 1;
                printf(" delete ----- \n");
            } else if (event->mask & IN_CREATE) {
                quit = 0;
                sprintf(strbuf, "/run/media/%s", event->name);
                printf(" IN_CREATE ----- %s\n", strbuf);
                pthread_create(&id, NULL, pthread_runting, strbuf);
            }

            tmp_len = sizeof(struct inotify_event) + event->len;
            event = (struct inotify_event *) (offset + tmp_len);
            offset += tmp_len;
        }
    }
}
