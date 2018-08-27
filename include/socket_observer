//
// Created by bingo on 18-7-28.
//

#ifndef GST_PLAY_USB_SCAN_H
#define GST_PLAY_USB_SCAN_H

#include <glib.h>
#define FD_SIZE (5)
typedef void (*FUNC)( gpointer userdata, char *mount);


typedef struct {
	char * str;
	int fd;
}write_buf_t;

typedef struct {
    gboolean  should_quit;
    int listen_socket;
    pthread_t id;
    FUNC mount;
    FUNC unmount_cb;
    gpointer userdata;
    int client[FD_SIZE];
    GList * write_buf;

}socket_observer_t;


socket_observer_t *usb_monitor_new(void);

void usb_monitior_blocked_notice(socket_observer_t * usb_monitor, gpointer userdata);


void usb_monitor_destor(socket_observer_t ** pp_usb_monitor);


#endif //GST_PLAY_USB_SCAN_H
