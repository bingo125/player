#include <unistd.h>
#include "usb_scan.h"


void mount_cb(gpointer userdata, char *mount){
    g_print("mount %s \n",mount);
}
void unmount_cb(gpointer userdata, char *mount){
    g_print("unmount_cb %s \n",mount);
}
int main()
{
    socket_observer_t *usb_monitor = usb_monitor_new();
    usb_monitior_blocked_notice(usb_monitor,NULL);
    sleep(200);
    usb_monitor_destor(&usb_monitor);
    return 0;
}