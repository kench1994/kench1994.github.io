#ifndef __PROTO_H__
#define __PROTO_H__

#include <sys/types.h>

#define CHANNEL_ID	0

typedef enum {
    IDLE = 0,
    STARTING,
    RUNNING,
    EXITING,
    EXITED,
    STOPPED,
} proc_status;

struct message {
    proc_status	status;
    pid_t		pid;
    int			worker_id;
};

#endif // __PROTO_H__