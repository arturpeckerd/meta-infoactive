#include <stdio.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include <signal.h>

#define MAX_SIZE 128

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            break; \
        } \
    } while (0) \


static mqd_t rcv_queue;

static void signal_handler(int sig)
{
    static int quit = 0;

    if (sig == SIGINT && !quit) {
        quit = 1;
        mq_close(rcv_queue);
        mq_unlink("ui_queue");
        rcv_queue=0;
    }
}


void main(void){
    char msg[MAX_SIZE] = "";
    char cmd[16];
    ssize_t len;
    struct mq_attr attr;
    long long position;
    long long length;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    signal(SIGINT, signal_handler);  // CRTL-C or kill -INT

    rcv_queue = mq_open("/ui_rcvqueue", O_CREAT | O_RDONLY, 0644, &attr);
    if (rcv_queue <= 0) {
        printf("UI - rcvqueue mq_open() failed %d : %s\n", errno, strerror(errno));
        return -1;
    }


    while (rcv_queue) {
        len = mq_receive(rcv_queue, msg, MAX_SIZE, NULL);
        CHECK((len >= 0) && (len <= MAX_SIZE));
        msg[len] = '\0';
        //printf("UI - received : %s\n", msg);
        if (strstr(msg, "MSG_UI")) {
            sscanf(msg,"%*s %lld %lld", &position, &length);
            printf("   Pos: %lld len:%lld \n", position/1000000, length/1000000);
        }
    }

    printf(" mq test ended.\n");
}
 
