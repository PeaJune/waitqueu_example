obj-m+=waitqueue_example.o

KERN_DIR=/lib/modules/`uname -r`/build/

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` clean
