//
// Created by bingo on 18-7-31.
//

#ifndef GST_PLAY_NET_IMP_H
#define GST_PLAY_NET_IMP_H

#include <glib.h>


#define MOUNT "VOLUME MOUNT="
#define UNMOUNT "VOLUME UNMOUNT="

struct _callback_t;

typedef char * (*func2)(  struct _callback_t *, char *,gpointer);

typedef struct _callback_t{
    gchar *match_id;
	const gchar *freeback;
	func2 function;
	gboolean  muti_char;
} callback_t;



callback_t * net_impl_cbs(void);

void net_impl_set_buf(GList** buf);

#endif //GST_PLAY_NET_IMP_H
