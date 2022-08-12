obj-m = Appdriver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	rm -rf *.o *.ko *.mod.* .c* .t*  .*.cmd modules.order Module.symvers

