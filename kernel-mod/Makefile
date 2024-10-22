SHELL = /bin/bash
.ONESHELL:
.SHELLFLAGS += -xe

FAMILY_NAME = netlink_proxy
GROUP_NAME = broadcast

obj-m = netlink_proxy_ko.o
EXTRA_CFLAGS=-I$(PWD)/../common/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install-module:
	sudo insmod netlink_proxy_ko.ko family_name=$(FAMILY_NAME) group_name=$(GROUP_NAME)

remove-module:
	sudo rmmod netlink_proxy_ko.ko
