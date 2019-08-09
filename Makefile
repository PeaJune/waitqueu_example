obj-m+=waitqueue_example.o

KERN_DIR=/lib/modules/4.4.0-31-generic/build/

all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	make -C $(KERN_DIR) M=`pwd` clean
