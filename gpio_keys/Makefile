
#build ko
KERDIR = /home/book/100ask_imx6ull-sdk/Linux-4.9.88
#CROSS_COMPILE = /home/book/100ask_imx6ull-sdk/ToolChain/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
CROSS_COMPILE = /home/book/100ask_imx6ull-sdk/ToolChain/arm-buildroot-linux-gnueabihf_sdk-buildroot/bin/arm-buildroot-linux-gnueabihf-


all : 
	make -C $(KERDIR) M=`pwd` CROSS_COMPILE=$(CROSS_COMPILE) ARCH=arm modules

clean :
	make -C $(KERDIR) M=`pwd` CROSS_COMPILE=$(CROSS_COMPILE) modules clean


obj-m += test_gpio_keys.o 