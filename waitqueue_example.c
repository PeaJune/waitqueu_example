#include <linux/module.h>  
#include <linux/init.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>  
#include <linux/uaccess.h>  
#include <linux/semaphore.h>  
#include <linux/cdev.h>  
#include <linux/device.h>  
#include <linux/ioctl.h>  
#include <linux/slab.h>  
#include <linux/errno.h>  
#include <linux/string.h>  
#include <linux/platform_device.h>
#include <linux/wait.h>

wait_queue_head_t mem_test_queue;
int data_valid = 0;
typedef struct wait_queue_entry wait_queue_entry_t;
static int user_defined_callback(wait_queue_entry_t *wait, unsigned mode, int sync, void *key)
{
	printk(KERN_WARNING "user-defined callback function.\n");	
	default_wake_function(wait, mode, sync, key);
	return 0;
}


static int mem_test_open(struct inode *inode, struct file *pfile)
{
	return 0;	
}

static int mem_test_release(struct inode *inode, struct file *pfile)
{
	return 0;
}
static ssize_t mem_test_read (struct file *pfile, char __user *buf, size_t count, loff_t *ppos)
{
	DECLARE_WAITQUEUE(mem_read_queue, current);
	mem_read_queue.func = user_defined_callback;
	//init_waitqueue_func_entry(&mem_read_queue, user_defined_callback);
	if(data_valid == 0)
	{
		add_wait_queue_exclusive(&mem_test_queue, &mem_read_queue);	
		//add_wait_queue(&mem_test_queue, &mem_read_queue);	
		set_current_state(TASK_INTERRUPTIBLE);
		printk(KERN_WARNING "invalid data....\n");
		schedule();
		printk(KERN_WARNING "valid data....\n");
	}
	printk(KERN_WARNING "read data....\n");
	data_valid=0;
	remove_wait_queue(&mem_test_queue, &mem_read_queue);
	return 0;
}

static ssize_t mem_test_write(struct file *pfile, const char __user *buf, size_t count, loff_t *ppos)
{	
	printk(KERN_WARNING "write data\n");
	data_valid = 1;
	wake_up(&mem_test_queue);
	return count;
}

struct file_operations mem_test_fops=
{
	.owner 	= THIS_MODULE,
	.open 	= mem_test_open,
	.read	= mem_test_read,
	.write	= mem_test_write,
	.release = mem_test_release,
};

unsigned int major = 205;
struct class *mem_test_class = NULL;

static int __init mem_test_init(void)
{
	int ret = 0;	

	ret = register_chrdev(major, "mem_test", &mem_test_fops);
	if(ret!=0)
	{
		printk(KERN_ERR "mem_test: register device failed!");
		return ret;
	}
	mem_test_class = class_create(THIS_MODULE, "mem_test");
	if(IS_ERR(mem_test_class))
		return -1;
	device_create(mem_test_class, NULL, MKDEV(major, 0), NULL, "mem_test");
	init_waitqueue_head(&mem_test_queue);
	printk(KERN_WARNING "mem test init.\n");

	return 0;
}

static void __exit mem_test_exit(void)
{
	device_destroy(mem_test_class, MKDEV(major, 0));
	class_destroy(mem_test_class);
	unregister_chrdev(major, "mem_test");	
	printk(KERN_WARNING "mem test exit.\n");
	return;
}

module_init(mem_test_init);
module_exit(mem_test_exit);
MODULE_LICENSE("GPL");
