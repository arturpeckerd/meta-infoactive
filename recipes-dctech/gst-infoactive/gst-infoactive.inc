SUMMARY = "GSTREAMER infoactive app"
DESCRIPTION = "GSTREAMER Infoactive app"
SECTION = "system"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=96af5705d6f64a88e035781ef00e98a8"

PR = "r0"

DEPENDS = "gstreamer1.0 glib-2.0"
RDEPENDS_${PN} = "gstreamer1.0 glib-2.0"

SRC_URI[md5sum] = ""
SRC_URI[sha256sum] = ""

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

inherit autotools pkgconfig

CFLAGS ="-I${STAGING_KERNEL_DIR}/include/uapi -I${STAGING_KERNEL_DIR}/include" 

do_install_append() {
        install -d ${D}/etc/rcS.d

        # app

        # binaries

        # management
        [ -h ${D}/etc/rcS.d/S90hmi ] || ln -s /etc/init.d/hmi.sh ${D}/etc/rcS.d/S90hmi
}

PACKAGES = "${PN} ${PN}-dbg ${PN}-dev ${PN}-staticdev"

FILES_${PN} += "${sysconfdir}/rcS.d/S90hmi /opt/crank/linux-imx6yocto-armle-opengles_2.0-obj/lib/*.so"
FILES_${PN} += "/opt/crank/linux-imx6yocto-armle-opengles_2.0-obj/bin"
FILES_${PN} += "/opt/crank/linux-imx6yocto-armle-opengles_2.0-obj/plugins"
FILES_${PN} += "/opt/crank/infoactive-hmi"
FILES_${PN} += "/opt/crank/bin"
FILES_${PN}-dev += "${includedir}"
FILES_${PN}-dev += "/opt/crank/linux-imx6yocto-armle-opengles_2.0-obj/include"
FILES_${PN}-staticdev += "/opt/crank/linux-imx6yocto-armle-opengles_2.0-obj/lib/*.a"
FILES_${PN}-dbg += "${bindir}/.debug"

