SUMMARY = "A small image just capable of allowing a device to boot."
LICENSE = "MIT"

inherit core-image
# image requirements
IMAGE_INSTALL += " libgcc ${ROOTFS_PKGMANAGE_BOOTSTRAP} ${CORE_IMAGE_EXTRA_INSTALL}"
IMAGE_INSTALL += " jpeg libpng"
IMAGE_INSTALL += " libgles2-mx6"
IMAGE_INSTALL += " gst-infoactive"
IMAGE_INSTALL += " libedit alsa-lib alsa-utils sqlite3"
IMAGE_INSTALL += " packagegroup-fslc-gstreamer1.0-full"

IMAGE_LINGUAS = " "

CONFLICT_DISTRO_FEATURES = "directfb"
