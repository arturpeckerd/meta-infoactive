#include <stdio.h>
#include <gst/gst.h>
#include <glib.h>
#include <string.h>
#include <pthread.h>
#include <mqueue.h>
#include <errno.h>
#include "commons.h"

static pthread_t ui_queue_thread;
static mqd_t ui_sndqueue;
static mqd_t ui_rcvqueue;
GMainLoop *loop;
gint64 len;

typedef struct _CustomData {
    GstElement *pipeline;
    GstElement *source;
    GstElement *capsfilter; 
    GstElement *demuxer;
    GstElement *video_queue; 
    GstElement *video_decoder; 
    GstElement *video_sink;
    GstElement *audio_queue; 
    GstElement *audio_decoder; 
    GstElement *audio_sink;
}GstElementsData;

GstElementsData gstElemData;

static inline void element_make(GstElement **gst_element, gchar *factory_name, gchar *name){
    *gst_element  = gst_element_factory_make (factory_name, name);
    if (!*gst_element) {
        g_printerr (" %s element could not be created.\n", factory_name);
    }  
}

static gboolean cb_print_position (GstElement *pipeline){
    gint64 pos;
    char msg[MAX_SIZE] = "";

    if (gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos)
                    && gst_element_query_duration (pipeline, GST_FORMAT_TIME, &len)) {
      //g_print ("Time: %" GST_TIME_FORMAT " / %" GST_TIME_FORMAT "\r", GST_TIME_ARGS (pos), GST_TIME_ARGS (len));

      snprintf(msg, sizeof(msg), "%s %f %"GST_TIME_FORMAT" %"GST_TIME_FORMAT,MSG_UI_POSITION_UPDATE, (float)pos/len, GST_TIME_ARGS (pos), GST_TIME_ARGS (len));
      mq_send(ui_rcvqueue, msg, strlen(msg), 0);
  }

  /* call me again */
  return TRUE;
}

static void cb_new_pad (GstElement *element, GstPad *pad, GstElementsData *gstElemData){
    gchar *name;
    name = gst_pad_get_name (pad);
    g_print ("A new pad %s was created\n", name);
    //GstCaps *caps=gst_pad_get_current_caps (gst_element_get_static_pad (gstElemData->capsfilter, "sink"));
    //g_print ("++++ cb_new_pad (caps): %s\n", gst_caps_to_string(caps));
    //gst_object_unref(caps);
    //print_pad_templates_information (element);
    if (strcmp(name,"video_0")==0){
        if (!gst_pad_is_linked (gst_element_get_static_pad (gstElemData->video_queue, "sink"))) {
	    if (gst_element_link_pads (element, "video_0", gstElemData->video_queue, "sink")){
                g_object_set (G_OBJECT (gstElemData->video_queue), "max-size-time", 0, NULL);
                g_print ("video_0 was created successful\n");
            }else
                g_print ("video_0 was not created\n");
        }
        else
            g_print ("%s already linked \n", name);
    }else if(strcmp(name,"audio_0")==0){
        if (!gst_pad_is_linked (gst_element_get_static_pad (gstElemData->audio_queue, "sink"))) {
	    if (gst_element_link_pads (element, "audio_0", gstElemData->audio_queue, "sink")){
                g_object_set (G_OBJECT (gstElemData->audio_queue), "max-size-time", 0, NULL);
                g_print ("audio_0 was created successful\n");
            }else
	        g_print ("audio_0 was not created\n");
        }
        else
            g_print ("%s already linked \n", name);
    }
    g_free (name);
}

static void ui_queue_process_message(const char *message)
{  
    char cmd[MAX_SIZE];
    float pos_norm;
    gint64 time_pos;

    if (strstr(message, "MSG_UI")) {
        sscanf(message, "%*s %s", cmd);
        if (strstr(cmd,"play")){
            printf("Play cmd\n");
            /* Set the pipeline to "playing" state*/
            //g_print ("Now playing: %s\n", argv[1]);
            gst_element_set_state (gstElemData.pipeline, GST_STATE_PLAYING);
        }else if (strstr(cmd,"pause")){
            printf("Pause cmd\n");
            gst_element_set_state (gstElemData.pipeline, GST_STATE_PAUSED);
        }else if (strstr(cmd,"stop")){
            printf("Stop cmd\n");
            gst_element_set_state (gstElemData.pipeline, GST_STATE_NULL);
            g_main_loop_quit (loop);
        }else if (strstr(cmd,"forward")){
            printf("Forward cmd\n");
        }else if (strstr(cmd,"backward")){
            printf("Backward cmd\n");
        }else if (strstr(cmd,"pwr")){
            printf("Power cmd\n");
            sscanf(message, "%*s %*s %s", cmd);
            if (strstr(cmd,"off"))
                g_main_loop_quit (loop);
        }else if (strstr(cmd,"seek")){
            printf("Seek_cmd\n");
            sscanf(message, "%*s %*s %f", &pos_norm);
            time_pos=pos_norm*(float)len;
            printf("msg: %s pos norm: %f time pos: %lld length: %lld \n", message, pos_norm,time_pos,len);
            if (!gst_element_seek (gstElemData.pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
                         GST_SEEK_TYPE_SET, time_pos,
                         GST_SEEK_TYPE_SET, len)){
            /*if (!gst_element_seek_simple (gstElemData.pipeline, GST_FORMAT_TIME,
                                                  GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_SEGMENT, time_pos)){*/

                g_print ("Seek failed!\n");
            }
        }
    }
}

static void* ui_queue_process (void *param)
{
    UNUSED(param);
    char buf[MAX_SIZE] = "";
    ssize_t len;

    printf("UI - ui_queue_process ...\n");

    while (ui_sndqueue) {
        len = mq_receive(ui_sndqueue, buf, MAX_SIZE, NULL);
        CHECK((len >= 0) && (len <= MAX_SIZE));
        buf[len] = '\0';
        printf("UI - received : %s\n", buf);
        ui_queue_process_message(buf);
    }

    return 0;
}


static int ui_queue_start (void)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    printf("UI - ui_queue_start ...\n");

    ui_sndqueue = mq_open(UI_SNDQUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (ui_sndqueue <= 0) {
        printf("UI - ui_sndqueue mq_open() failed %d : %s\n", errno, strerror(errno));
        return -1;
    }

    ui_rcvqueue = mq_open(UI_RCVQUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
    if (ui_rcvqueue <= 0) {
        printf("UI - ui_rcvqueue mq_rcvopen() failed %d : %s\n", errno, strerror(errno));
        return -1;
    }

    if (pthread_create(&ui_queue_thread, NULL, ui_queue_process, NULL) != 0) {
        printf("UI - pthread_create() failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}


static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data){
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("End of stream\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);
        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);
        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
  }
  return TRUE;
}

int main (int argc, char *argv[]){
    GstBus *bus;
    guint bus_watch_id;
    ui_queue_start();	
    /* Initialisation */
    gst_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* Check input arguments */
    if (argc != 2) {
        g_printerr ("Usage: %s AudioFile\n", argv[0]);
        return -1;
    }

    /* Create gstreamer elements */
    gstElemData.pipeline = gst_pipeline_new ("audio-player");
    if (!gstElemData.pipeline) {
        g_printerr (" Pipeline element could not be created. Exiting.\n");
        return -1;
    }
    element_make(&gstElemData.source, "filesrc", "my-filesrc");
    element_make(&gstElemData.capsfilter, "capsfilter", "my-capsfilter");
    element_make(&gstElemData.demuxer, "aiurdemux", "my-aiurdemux");
    element_make(&gstElemData.video_queue, "queue", "my-vqueue");
    element_make(&gstElemData.video_decoder, "vpudec", "my-vpudec");
    element_make(&gstElemData.video_sink, "imxv4l2sink", "my-imxv4l2sink ");
    element_make(&gstElemData.audio_queue, "queue", "my-aqueue");
    element_make(&gstElemData.audio_decoder, "beepdec", "my-beepdec");
    element_make(&gstElemData.audio_sink, "alsasink", "my-alsasink");

    if (!gstElemData.source || !gstElemData.capsfilter || !gstElemData.demuxer || !gstElemData.video_queue || !gstElemData.video_decoder 
	    || !gstElemData.video_sink || !gstElemData.audio_queue || !gstElemData.audio_decoder || !gstElemData.audio_sink) {
        g_printerr (" Error: Some element could not be created. Exiting.\n");
        return -1;
    }
    /* we set the input filename to the source element */
    g_object_set(G_OBJECT (gstElemData.video_sink),"device","/dev/video16",NULL);
    g_object_set (G_OBJECT (gstElemData.source), "location", argv[1], NULL);
    /* we add a message handler */
    bus = gst_pipeline_get_bus (GST_PIPELINE (gstElemData.pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);
  
    gst_bin_add_many (GST_BIN (gstElemData.pipeline), gstElemData.source, gstElemData.capsfilter, 
        gstElemData.demuxer, gstElemData.video_queue, gstElemData.video_decoder, gstElemData.video_sink, 
        gstElemData.audio_queue, gstElemData.audio_decoder, gstElemData.audio_sink, NULL);
    gst_element_link_many (gstElemData.source, gstElemData.capsfilter, gstElemData.demuxer, NULL);
	/* Video Links*/
    gst_element_link_many (gstElemData.video_queue, gstElemData.video_decoder, gstElemData.video_sink, NULL);
	/* Audio Links*/
    gst_element_link_many (gstElemData.audio_queue, gstElemData.audio_decoder, gstElemData.audio_sink, NULL);

	
    g_signal_connect (gstElemData.demuxer, "pad-added", G_CALLBACK (cb_new_pad), &gstElemData);
  
    GstCaps *incaps = gst_caps_new_empty_simple ("video/quicktime");
    //GstCaps *outcaps, *incaps;
    //gst_caps_set_simple (incaps, "video/quicktime", NULL);
    g_object_set (gstElemData.capsfilter, "caps", incaps, NULL);
    gst_caps_unref (incaps);
    //g_object_get (gstElemData.capsfilter,"caps", &outcaps,NULL);
    //g_print ("String capabilities (capsfilter): %s\n", gst_caps_to_string(outcaps));
    //st_object_unref (outcaps);
	

    /* Iterate */
    g_print ("Running...\n");
	g_timeout_add (200, (GSourceFunc) cb_print_position, gstElemData.pipeline);
    g_main_loop_run (loop);

    /* Out of the main loop, clean up nicely */
    g_print ("Returned, stopping playback\n");
    gst_element_set_state (gstElemData.pipeline, GST_STATE_NULL);

    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (gstElemData.pipeline));
    g_source_remove (bus_watch_id);
    g_main_loop_unref (loop);

    return 0;
}
