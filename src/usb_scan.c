//
// Created by bingo on 18-7-28.

#include "../include/usb_scan.h"

#include "libue.h"

const char* uev_action_str[] = { "invalid", "add", "remove", "change", "move", "online", "offline" };

static inline void ue_dump_event(struct uevent *uevp) {
    printf("%s %s\n", uev_action_str[uevp->action], uevp->devpath);
}
/*
 * Reference for uevent format:
 * https://www.kernel.org/doc/pending/hotplug.txt
 */
int ue_parse_event_msg(struct uevent *uevp, size_t buflen) {
    /* skip udev events */
    if (memcmp(uevp->buf, "libudev", 8) == 0) return ERR_PARSE_UDEV;

    /* validate message header */
    size_t body_start = strlen(uevp->buf) + 1;
    if (body_start < sizeof("a@/d")
        || body_start >= buflen
        || (strstr(uevp->buf, "@/") == NULL)) {
        return ERR_PARSE_INVALID_HDR;
    }

    int i = body_start;
    char *cur_line;
    uevp->buflen = buflen;

    while (i < buflen) {
        cur_line = uevp->buf + i;
        UE_DEBUG("line: '%s'\n", cur_line);
        if (UE_STR_EQ(cur_line, "ACTION")) {
            cur_line += sizeof("ACTION");
            if (UE_STR_EQ(cur_line, "add")) {
                uevp->action = UEVENT_ACTION_ADD;
            } else if (UE_STR_EQ(cur_line, "change")) {
                uevp->action = UEVENT_ACTION_CHANGE;
            } else if (UE_STR_EQ(cur_line, "remove")) {
                uevp->action = UEVENT_ACTION_REMOVE;
            } else if (UE_STR_EQ(cur_line, "move")) {
                uevp->action = UEVENT_ACTION_MOVE;
            } else if (UE_STR_EQ(cur_line, "online")) {
                uevp->action = UEVENT_ACTION_ONLINE;
            } else if (UE_STR_EQ(cur_line, "offline")) {
                uevp->action = UEVENT_ACTION_OFFLINE;
            }
        } else if (UE_STR_EQ(cur_line, "DEVPATH")) {
            uevp->devpath = cur_line + sizeof("DEVPATH");
        }
        /* proceed to next line */
        i += strlen(cur_line) + 1;
    }
    return 0;
}



static inline void ue_reset_event(struct uevent *uevp) {
    uevp->action = UEVENT_ACTION_INVALID;
    uevp->buflen = 0;
    uevp->devpath = NULL;
}

int ue_init_listener(struct uevent_listener *l) {
    memset(&l->nls, 0, sizeof(struct sockaddr_nl));
    l->nls.nl_family = AF_NETLINK;
    l->nls.nl_pid = getpid();
    l->nls.nl_groups = -1;

    l->pfd.events = POLLIN;
    l->pfd.fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (l->pfd.fd == -1) return ERR_LISTENER_NOT_ROOT;

    if (bind(l->pfd.fd, (struct sockaddr*)&(l->nls), sizeof(struct sockaddr_nl))) {
        return ERR_LISTENER_BIND;
    }

    return 0;
}

int ue_wait_for_event(struct uevent_listener *l, struct uevent *uevp) {
    ue_reset_event(uevp);
    while (poll(&(l->pfd), 1, -1) != -1) {
        int i, len = recv(l->pfd.fd, uevp->buf, sizeof(uevp->buf), MSG_DONTWAIT);
        if (len == -1) return ERR_LISTENER_RECV;
        if (ue_parse_event_msg(uevp, len) == 0) {
            UE_DEBUG("uevent successfully parsed\n");
            return 0;
        } else {
            UE_DEBUG("skipped unsupported uevent:\n%s\n", uevp->buf);
        }
    }
    return ERR_LISTENER_POLL;
}

usb_monitor_t *usb_monitor_new() {
    usb_monitor_t *monitor = g_new(usb_monitor_t, 1);
    g_assert(monitor != NULL);
    ue_init_listener(&monitor->listener);
    monitor->should_quit = FALSE;
    return monitor;
}



void usb_monitor_destor(usb_monitor_t **pp_usb_monitor) {
    g_free(*pp_usb_monitor);
    *pp_usb_monitor = NULL;
}

static gboolean  parse( gchar * buf, const gchar * line){
    GRegex* regex;
    GMatchInfo *match_info;
    GError *error = NULL;
    regex = g_regex_new("sd[b-zB-Z]\\d$", 0 , 0, NULL);
    g_regex_match(regex, line, 0, &match_info);
    while (g_match_info_matches(match_info)) {
        gchar* word = g_match_info_fetch(match_info, 0);
        g_print("-----------------------%s\n",word);
//        sprintf("/dev/%s", )
        g_free(word);
        g_match_info_next(match_info, NULL);
    }
    g_match_info_free(match_info);
    g_regex_unref(regex);
}

void usb_monitior_blocked_notice(usb_monitor_t * usb_monitor, FUNC mount_cb, FUNC unmount_cb, gpointer userdata){
    int re, len;
    char * pch;
    struct uevent uev;
    while(!usb_monitor->should_quit)
    {
        if((re = ue_wait_for_event(&usb_monitor->listener, &uev)) == 0){
            switch( uev.action){
                case UEVENT_ACTION_ADD:
                    printf("UEVENT_ACTION_ADD  %s \n ",  uev.devpath);
                    parse(NULL,uev.devpath);
                    break;
                case UEVENT_ACTION_CHANGE:
                    printf("UEVENT_ACTION_CHANGE  %s \n ",  uev.buf);
                    break;
                default:
                    break;
            }
        }
    }
}