/*
 ============================================================================
 Name        : hmi_sbio.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : hmi_sbio C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include "gre/greio.h"
#include <mqueue.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "commons.h"

static pthread_t ui_rcvqueue_thread;
static mqd_t ui_sndqueue;
static mqd_t ui_rcvqueue;
static unsigned int ui_quit = 0;

typedef struct {
    long long int position;
    long long int length;
} track_msg_data_t;


static void* ui_rcvqueue_process (void *param)
{
    UNUSED(param);
    gre_io_t *send_handle;
    gre_io_serialized_data_t *md_buffer = NULL;
    char msg[MAX_SIZE] = "";
    char song_pos[64]="";
    char song_len[64]="";
    float pos_norm;
    ssize_t len;
    //track_msg_data_t track_data;

    send_handle = gre_io_open("input_data", GRE_IO_TYPE_WRONLY);
    if(send_handle == NULL) {
        printf("Can't open send handle\n");
        return NULL;
    }

    printf("UI - ui_rvcqueue_process ...\n");

    while (ui_rcvqueue) {
        len = mq_receive(ui_rcvqueue, msg, MAX_SIZE, NULL);
        CHECK((len >= 0) && (len <= MAX_SIZE));
        msg[len] = '\0';
        printf("UI - received : %s\n", msg);
        if (strstr(msg, MSG_UI_POSITION_UPDATE)) {
            sscanf(msg,"%*s %f %s %s",&pos_norm, &song_pos ,&song_len );

	    md_buffer = gre_io_serialize(md_buffer, NULL,"INPUT_EVENT","4f1 pos", &pos_norm, sizeof(pos_norm));
	    gre_io_send(send_handle, md_buffer);
        }
    }

    return 0;
}


static int ui_rcvqueue_start(void)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    printf("UI - ui_rcvqueue_start ...\n");

    ui_rcvqueue = mq_open(UI_RCVQUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (ui_rcvqueue <= 0) {
        printf("UI - ui_rcvqueue mq_open() failed %d : %s\n", errno, strerror(errno));
        return -1;
    }

    if (pthread_create(&ui_rcvqueue_thread, NULL, ui_rcvqueue_process, NULL) != 0) {
        printf("UI - pthread_create() failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

static void ui_stop (void)
{
    printf("- close ui queues.\n");
    mq_close(ui_sndqueue);
    mq_unlink(UI_SNDQUEUE_NAME);
    ui_sndqueue=0;
    mq_close(ui_rcvqueue);
    mq_unlink(UI_RCVQUEUE_NAME);
    ui_rcvqueue=0;
}


static void signal_handler(int sig)
{
    static int quit = 0;

    if (sig == SIGINT && !quit) {
        quit = 1;
        ui_stop();
        ui_quit=1;
    }
}


int main(void) {
    //gre_io_t                 *send_handle;
    gre_io_t                 *recv_handle;
    gre_io_serialized_data_t      *buffer = NULL;
    //gre_io_serialized_data_t    *md_buffer = NULL;
    int ret;
    char                   *revent_name;
    char                   *revent_target;
    char                   *revent_format;
    uint8_t                *revent_data;
    int                    rnbytes;
    //char str[]="POWER BOTTOM PRESSED";
    //***********************
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
    char cmd[MAX_SIZE];

    signal(SIGINT, signal_handler);  // CRTL-C or kill -INT

    ui_sndqueue = mq_open(UI_SNDQUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (ui_sndqueue <= 0) {
        printf("wfd_sink - ui mq_open() failed %d : %s", errno, strerror(errno));
        exit(1);
    }
    //*********************
    /*send_handle = gre_io_open("input_data", GRE_IO_TYPE_WRONLY);
    if(send_handle == NULL) {
        printf("Can't open send handle\n");
        return 0;
    }*/

    recv_handle = gre_io_open("media_player", GRE_IO_TYPE_RDONLY);
    if(recv_handle == NULL) {
        printf("Can't open recv handle\n");
        return 0;
    }

    if (ui_rcvqueue_start() < 0) {
        printf("UI rcvqueue - start failed ...\n");
        return 0;
    }


    while(!ui_quit) {
        ret = gre_io_receive(recv_handle, &buffer);
        if(ret < 0) {
            printf("Problem receiving data on channel\n");
            break;
        }

        rnbytes = gre_io_unserialize(buffer, &revent_target,
                    &revent_name, &revent_format, (void **)&revent_data);

        if(strcmp("av_player_ctrl",(char *)revent_name) == 0){
            printf("Setting media = %s\n",revent_data);
            if(strcmp("play",(char *)revent_data) == 0){
                printf("Play \n");
                snprintf(cmd, sizeof(cmd), "MSG_UI play");
                mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
            }else if(strcmp("pause",(char *)revent_data) == 0){
                printf("Pause \n");
                snprintf(cmd, sizeof(cmd), "MSG_UI pause");
                mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
            }else if(strcmp("stop",(char *)revent_data) == 0){
                printf("Stop \n");
                snprintf(cmd, sizeof(cmd), "MSG_UI stop");
                mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
            }else if(strcmp("backward",(char *)revent_data) == 0){
                printf("Backward \n");
            }else if(strcmp("forward",(char *)revent_data) == 0){
                printf("Forward \n");
            }else if(strcmp("repeat",(char *)revent_data) == 0){
                printf("Repeat \n");
            }else if(strcmp("pwr",(char *)revent_data) == 0){
                printf("Power \n");
                if (strstr((char *)&revent_data[4],"on" ))
                    system("gst-infoactive /home/root/Wildlife.mp4 &");
                if (strstr((char *)&revent_data[4],"off" )){
                    snprintf(cmd, sizeof(cmd), "MSG_UI pwr off");
                    mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
                }
                //md_buffer = gre_io_serialize(md_buffer, NULL,"INPUT_EVENT","1s0 str", str,sizeof(str));
                //gre_io_send(send_handle, md_buffer);
            }else if(strcmp("mute",(char *)revent_data) == 0){
                printf("Mute \n");
                snprintf(cmd, sizeof(cmd), "MSG_UI mute");
                mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
            }else if(strcmp("av_vol_ctrl",(char *)revent_name) == 0){
                printf("volumen = %f\n",*(float*)revent_data);
            }else if(strcmp("av_eq_ctrl",(char *)revent_name) == 0){
                printf("Format = %s\n",revent_format);
                printf("level = %f\n",*(float*)revent_data);
            }else if(strcmp("av_progress_ctrl",(char *)revent_name) == 0){
                printf("Format = %s\n",revent_format);
                printf("level = %f\n",*(float*)revent_data);
                snprintf(cmd, sizeof(cmd), "MSG_UI seek %f",*(float*)revent_data);
                mq_send(ui_sndqueue, cmd, strlen(cmd), 0);
            }
        }else{
            printf("Unknown event received : %s\n",(char *)revent_name);
        }
    }
    ui_stop();

    return 1;
}
