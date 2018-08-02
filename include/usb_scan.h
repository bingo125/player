//
// Created by bingo on 18-7-28.
//

#ifndef GST_PLAY_USB_SCAN_H
#define GST_PLAY_USB_SCAN_H

#include <glib.h>
#define FD_SIZE (5)
typedef void (*FUNC)( gpointer userdata, char *mount);

typedef struct {
    gboolean  should_quit;
    int listen_socket;
    pthread_t id;
    FUNC mount;
    FUNC unmount_cb;
    gpointer userdata;
    int client[FD_SIZE];
}usb_monitor_t;



static void *pthread_runting(void *);

void usb_monitior_blocked_notice(usb_monitor_t * usb_monitor, FUNC mount_cb, FUNC unmount_cb, gpointer userdata);


usb_monitor_t * usb_monitor_new();
void usb_monitor_destor(usb_monitor_t ** pp_usb_monitor);
#endif //GST_PLAY_USB_SCAN_H
static void handle_message(usb_monitor_t * usb_monitor, const char * message);