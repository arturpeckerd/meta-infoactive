
include_HEADERS = commons.h

bin_PROGRAMS = gst-infoactive gst-cmd gst-rcv

gst_infoactive_SOURCES = gst-infoactive.c
gst_infoactive_CFLAGS = $(AM_CFLAGS) -I. $(GST_CFLAGS)  @DEFS@
gst_infoactive_CPPFLAGS =
gst_infoactive_LDFLAGS = $(GST_LIBS) -lpthread -lrt 
gst_infoactive_LDADD =

gst_cmd_SOURCES = gst-cmd.c
gst_cmd_CFLAGS = $(AM_CFLAGS) -I. @DEFS@
gst_cmd_CPPFLAGS =
gst_cmd_LDFLAGS = -lpthread -lrt
gst_cmd_LDADD =

gst_rcv_SOURCES = gst-rcv.c
gst_rcv_CFLAGS = $(AM_CFLAGS) -I. @DEFS@
gst_rcv_CPPFLAGS =
gst_rcv_LDFLAGS = -lpthread -lrt
gst_rcv_LDADD = 

