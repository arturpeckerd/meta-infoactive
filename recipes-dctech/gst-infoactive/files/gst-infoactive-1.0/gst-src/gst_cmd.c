#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include "commons.h"

static mqd_t ui_queue;

void main(int argc , int **argv){
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
    char cmd[128];

    printf("Sending %s command to gstreamer \n", argv[1]);

    ui_queue = mq_open(UI_SNDQUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (ui_queue <= 0) {
        printf(" - ui mq_open() failed %d : %s\n", errno, strerror(errno));
        exit(1);
    }


    snprintf(cmd, sizeof(cmd), "MSG_UI %s", argv[1]);
 
    mq_send(ui_queue, cmd, strlen(cmd), 0);
    mq_close(ui_queue);


}
