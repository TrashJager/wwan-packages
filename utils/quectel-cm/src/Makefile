ifneq ($(CROSS_COMPILE),)
CROSS-COMPILE:=$(CROSS_COMPILE)
endif
#CROSS-COMPILE:=/workspace/buildroot/buildroot-qemu_mips_malta_defconfig/output/host/usr/bin/mips-buildroot-linux-uclibc-
#CROSS-COMPILE:=/workspace/buildroot/buildroot-qemu_arm_vexpress_defconfig/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabi-
#CROSS-COMPILE:=/workspace/buildroot-git/qemu_mips64_malta/output/host/usr/bin/mips-gnu-linux-
ifeq ($(CC),cc)
CC:=$(CROSS-COMPILE)gcc
endif
LD:=$(CROSS-COMPILE)ld

QL_CM_SRC=main.c   util.c  device.c
QL_CM_SRC+=atc.c atchannel.c at_tok.c
#QL_CM_SRC+=qrtr.c rmnetctl.c
ifeq (1,1)
QL_CM_DHCP=udhcpc.c
else
LIBMNL=libmnl/ifutils.c libmnl/attr.c libmnl/callback.c libmnl/nlmsg.c libmnl/socket.c
DHCP=libmnl/dhcp/dhcpclient.c libmnl/dhcp/dhcpmsg.c libmnl/dhcp/packet.c
QL_CM_DHCP=udhcpc_netlink.c
QL_CM_DHCP+=${LIBMNL}
endif

CFLAGS += -Wall -Wextra -Werror -O1 #-s
LDFLAGS += -lpthread -ldl -lrt

release: clean atc-proxy #qrtr-proxy
	$(CC) ${CFLAGS} ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-CM ${LDFLAGS}

debug: clean
	$(CC) ${CFLAGS} -g -DCM_DEBUG ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-CM -lpthread -ldl -lrt

atc-proxy:
	$(CC) ${CFLAGS} quectel-atc-proxy.c atchannel.c at_tok.c util.c -o quectel-atc-proxy ${LDFLAGS} 

clean:
	rm -rf *.o libmnl/*.o quectel-CM quectel-atc-proxy
