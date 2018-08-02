//
// Created by bingo on 18-7-30.
//


#include "usb_scan.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <memory.h>
#include "net_impl.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


typedef void (*FUNC)(gpointer userdata, char *mount);

void usb_monitior_blocked_notice(usb_monitor_t *usb_monitor, FUNC mount_cb, FUNC unmount_cb, gpointer userdata);

usb_monitor_t *usb_monitor_new();

void usb_monitor_destor(usb_monitor_t **pp_usb_monitor);

#define PORT 9990   //端口号
#define SIZE 1024   //定义的数组大小


static int Creat_socket()    //创建套接字和初始化以及监听函数
{
    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);   //创建一个负责监听的套接字
    if (listen_socket == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;  /* Internet地址族 */
    addr.sin_port = htons(PORT);  /* 端口号 */
    addr.sin_addr.s_addr = htonl(INADDR_ANY);   /* IP地址 */

    int on = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
        perror("setsockopt ");
    }
    int ret = bind(listen_socket, (struct sockaddr *) &addr, sizeof(addr));  //连接
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    ret = listen(listen_socket, 5);        //监听
    if (ret == -1) {
        perror("listen");
        return -1;
    }
    return listen_socket;
}


usb_monitor_t *usb_monitor_new() {
    usb_monitor_t *usb_monitor = g_new0(usb_monitor_t, 1);
    usb_monitor->listen_socket = Creat_socket();
    memset(&usb_monitor->client, -1, sizeof(usb_monitor->client));
    return usb_monitor;
}

void usb_monitior_blocked_notice(usb_monitor_t *usb_monitor, FUNC mount_cb, FUNC unmount_cb, gpointer userdata) {
    usb_monitor->mount = mount_cb;
    usb_monitor->unmount_cb = unmount_cb;
    usb_monitor->userdata = userdata;
    pthread_create(&usb_monitor->id, NULL, pthread_runting, usb_monitor);
    usb_monitor->should_quit = FALSE;
}

void usb_monitor_destor(usb_monitor_t **pp_usb_monitor) {

    (*pp_usb_monitor)->should_quit = TRUE;
    pthread_join((*pp_usb_monitor)->id, NULL);

    for (int i = 0; i < FD_SIZE; i++) {
        int *p_fd = &(*pp_usb_monitor)->client[i];
        if (*p_fd != -1) {
            close(*p_fd);
            *p_fd = -1;
        }
    }
    close((*pp_usb_monitor)->listen_socket);
    g_free(*pp_usb_monitor);
    *pp_usb_monitor = NULL;
}

void *pthread_runting(void *userdata) {
    usb_monitor_t *usb_monitor = (usb_monitor_t *) userdata;
    struct sockaddr_in cliaddr;
    int addrlen = sizeof(cliaddr);
    int client_socket;
    char buf[1024];
    int ret, i;
    fd_set fds;
    int client_fd;
    FD_ZERO(&fds);
    FD_SET(usb_monitor->listen_socket, &fds);
    int maxfd = usb_monitor->listen_socket;
    struct timeval time_lapse;

    while (!usb_monitor->should_quit) {
        fd_set tmp_fds = fds;
        time_lapse.tv_sec = 0;
        time_lapse.tv_usec = 300;

        switch (select(maxfd + 1, &tmp_fds, NULL, NULL, &time_lapse)) {
            case 0:
//                printf(" time is time_lapse and try again \n");
                break;
            case -1:
                perror(" xxxxxx");
                break;
            default:
                if (FD_ISSET(usb_monitor->listen_socket, &tmp_fds)) {
                    client_fd = accept(usb_monitor->listen_socket, (struct sockaddr *) &cliaddr, &addrlen);
                    for (i = 0; i < FD_SIZE; i++) {
                        if (usb_monitor->client[i] == -1) {
                            usb_monitor->client[i] = client_fd;
                            FD_SET(client_fd, &fds);
                            if (client_fd > maxfd) {
                                maxfd = client_fd;
                            }
                            break;
                        }
                    }
                    if (i == FD_SIZE) {
                        close(client_fd);
                        // 链接端口太多
                    }
                } else {
                    for (i = 0; i < FD_SIZE; i++) {
                        if (usb_monitor->client[i] == -1)
                            continue;
                        if (FD_ISSET(usb_monitor->client[i], &tmp_fds)) {
                            ret = recv(usb_monitor->client[i], buf, SIZE - 1, MSG_DONTWAIT);
                            if (ret <= 0) {
                                if (errno) {
                                    perror(" failure  ");
                                }
                                printf(" ret = %d  \n", ret);
                                close(usb_monitor->client[i]);
                                FD_CLR(usb_monitor->client[i], &fds);
                                usb_monitor->client[i] = -1;
                            } else {
                                buf[ret] = '\0';
                                if (buf[ret - 1] == '\n') {
                                    buf[ret - 1] = '\0';
                                }
                                printf("%d---------%s \n", ret, buf);
                                handle_message(usb_monitor, buf);
                            }
                        }
                    }
                }
        }
    }
}

#if 0
//todo
typedef struct  {
    gchar * match_id;
    FUNC  callback_func;
    void (*destroy)(gpointer * callback);
}callback_t;

callback_t *
#endif

typedef void (*func1)(gpointer);

typedef void (*func2)(gpointer, const char *);

static gboolean call_back_match(const callback_t * p_cb, const gchar * message, int *pi){
    int len = strlen(p_cb->match_id);
    int i = 0;
    for (; p_cb->match_id[i]&&i < len; i++) {
        if (p_cb->match_id[i] != message[i]) {

            return FALSE;
        }
    }
    *pi = i;
    return TRUE;
}
static void handle_message(usb_monitor_t *usb_monitor, const char *message) {
    gboolean ret;
    int i;
    const callback_t *ptr_cb = net_impl_cbs();
    gboolean flag ;
    while (ptr_cb->function != NULL) {
        int len = strlen(ptr_cb->match_id);
        flag = TRUE;
        if(call_back_match(ptr_cb, message, &i)){
            if(ptr_cb->function){
                if (!ptr_cb->n_args) {
                     ((func1) (ptr_cb->function))(usb_monitor->userdata);
                } else {
                     ((func2) (ptr_cb->function))(usb_monitor->userdata, &message[i]);
                }
            }
            return;
        }
        ptr_cb ++;
    }

    g_print("unspect message %s \n", message);
}