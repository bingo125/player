/*
    The MIT License (MIT)
    Copyright (c) <2016> <Qingping Hou>
    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to do
    so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef _LIBUE_H_
#define _LIBUE_H_

#include <sys/poll.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define LIBUE_VERSION_MAJOR "0"
#define LIBUE_VERSION_MINOR "2.0"
#define LIBUE_VERSION LIBUE_VERSION_MAJOR "." LIBUE_VERSION_MINOR
#define LIBUE_VERSION_NUMBER 10000
#ifndef DEBUG
#define DEBUG 0
#endif
#define UE_DEBUG(...) \
            do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while(0)

struct uevent_listener {
    struct pollfd pfd;
    struct sockaddr_nl nls;
};

#define ERR_LISTENER_NOT_ROOT -1
#define ERR_LISTENER_BIND -2
#define ERR_LISTENER_POLL -3
#define ERR_LISTENER_RECV -4
#define ERR_PARSE_UDEV -1
#define ERR_PARSE_INVALID_HDR -2
#define UE_STR_EQ(str, const_str) (strncmp((str), (const_str), sizeof(const_str)-1) == 0)

enum uevent_action {
    UEVENT_ACTION_INVALID = 0,
    UEVENT_ACTION_ADD,
    UEVENT_ACTION_REMOVE,
    UEVENT_ACTION_CHANGE,
    UEVENT_ACTION_MOVE,
    UEVENT_ACTION_ONLINE,
    UEVENT_ACTION_OFFLINE,
};

struct uevent {
    enum uevent_action action;
    char *devpath;
    char buf[4096];
    size_t buflen;
};
#endif

