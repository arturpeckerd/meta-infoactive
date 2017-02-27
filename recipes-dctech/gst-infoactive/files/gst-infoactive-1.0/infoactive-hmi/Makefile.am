
hmi_sbio_bindir = "/opt/crank/bin"
hmi_sbio_bin_PROGRAMS = hmi_sbio

hmi_sbio_SOURCES = hmi_sbio.c
hmi_sbio_CFLAGS = $(AM_CFLAGS) -I@HOSTDIR@/gst-src @DEFS@
hmi_sbio_CPPFLAGS =
hmi_sbio_LDFLAGS = -lgreio -lpthread -lrt
hmi_sbio_LDADD = -L@HOSTDIR@/@SB_LIB_DIR@ 

hmi_sbio_servicedir = $(sysconfdir)/init.d
hmi_sbio_service_SCRIPTS = hmi.sh

#bin_SCRIPTS = hmi

hmi_gappdir = "/opt/crank/infoactive-hmi"
hmi_gapp_DATA = infoactive-hmi.gapp 

hmi_imagesdir = "/opt/crank/infoactive-hmi/images"
hmi_images_DATA = \
    images/_0000_tick_6.png \
    images/_0001_tick_5.png \
    images/_0002_tick_4.png \
    images/_0003_tick_3.png \
    images/_0004_tick_2.png \
    images/_0005_tick_1.png \
    images/backward_btn_off.png \
    images/backward_btn_on.png \
    images/backward_btn.png \
    images/eq_base.png \
    images/eq_btn_off.png \
    images/eq_btn_on.png \
    images/forward_btn_off.png \
    images/forward_btn_on.png \
    images/forward_btn.png \
    images/home_off.png \
    images/home_on.png \
    images/lower_gate_bkg.png \
    images/media_player_btn_off.png \
    images/media_player_btn_on.png \
    images/mute_btn_off.png \
    images/mute_btn_on.png \
    images/mute_btn.png \
    images/pause_btn_off.png \
    images/pause_btn_on.png \
    images/pause_btn.png \
    images/play_btn_off.png \
    images/play_btn_on.png \
    images/player_framework.png \
    images/pwr_btn_off.png \
    images/pwr_btn_on.png \
    images/pwr_btn.png \
    images/repeat_btn_off.png \
    images/repeat_btn_on.png \
    images/repeat_btn.png \
    images/slider_4.1.png \
    images/slider_4.png \
    images/slider_base_4.1.png \
    images/stop_btn_off.png \
    images/stop_btn_on.png \
    images/stop_btn.png \
    images/trackingbar_1.png \
    images/trackingbar_base_1.png \
    images/upper_gate_bkg.png

hmi_scriptsdir = "/opt/crank/infoactive-hmi/scripts"
hmi_scripts_DATA = \
    scripts/callbacks.lua

hmi_eventsdir = "/opt/crank/infoactive-hmi/events"
hmi_events_DATA = \
    events/custom.evt 

hmi_templatesdir = "/opt/crank/infoactive-hmi/templates"
hmi_fontsdir = "/opt/crank/infoactive-hmi/fonts"

