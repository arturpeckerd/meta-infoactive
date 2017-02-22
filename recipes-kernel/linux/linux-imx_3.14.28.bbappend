FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:${THISDIR}/${PN}:"

PATCHTOOL = "git"

SRC_URI += "    file://fragment_mqueue.cfg \
                file://fragment_ath9k.cfg \
           "

do_copy_defconfig_prepend () {
    cat ${WORKDIR}/fragment*.cfg >> ${S}/arch/arm/configs/imx_v7_defconfig
}

