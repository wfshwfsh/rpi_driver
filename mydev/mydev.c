#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>


#define DRIVER_NAME "mydev"
const int num_of_dev = 1;
static int major;
static struct cdev mydev_cdev;
static struct class  *mydev_class;
static struct device *mydev_dev;


static int mydev_open(struct inode *inode, struct file *file)
{
	printk("[%s] \n", __func__);

	return 0;
}

static ssize_t mydev_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	printk("[%s] \n", __func__);
	char val[7]={'0','1','2','3','4','5','6'};

	//copy_to_user(buf, );
	copy_to_user(buf, val, sizeof(val));
	return sizeof(val);
}

static ssize_t mydev_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	printk("[%s] \n", __func__);

	//copy_from_user

	return 0;
}


static int mydev_close(struct inode *inode, struct file *file)
{
	printk("[%s] \n", __func__);

	return 0;
}



static struct file_operations mydev_fops = {
	.owner   = THIS_MODULE,
	.open    = mydev_open,
	.write   = mydev_write,
	.read    = mydev_read,
	.release = mydev_close,

};


static int mydev_init(void)
{
	printk("[%s] \n", __func__);
	int alloc_ret = 0;
	int cdev_ret = 0;
	
	//1. register char device => return "major" main device num
	dev_t dev = MKDEV(/*major*/0, /*minor*/0);
	
	alloc_ret = alloc_chrdev_region(&dev, 0, num_of_dev, DRIVER_NAME);
	if(IS_ERR(alloc_ret))
		goto ERR;
	
	major = MAJOR(dev);

	mydev_class = class_create(THIS_MODULE, "mydev");
	if(IS_ERR(mydev_class))
		goto ERR;

	//3. create class device
	//mydev_class_dev = class_device_create(mydev_class, NULL, MKDEV(major, 0), NULL, "mydev");
	mydev_dev = device_create(mydev_class, NULL, MKDEV(major, 0), NULL, "mydev");
	if(IS_ERR(mydev_dev))
        goto ERR;

	cdev_init(&mydev_cdev, &mydev_fops);
    cdev_ret = cdev_add(&mydev_cdev, dev, num_of_dev);
    if (cdev_ret)
		goto ERR;

	//4. ioremap for physical address, if needed
	return 0;
	
ERR:
	if (cdev_ret == 0)
		cdev_del(&mydev_cdev);
	
	if(!mydev_class)
		device_destroy(mydev_class, dev);
	
	if(!mydev_dev)
		class_destroy(mydev_class);

	if (alloc_ret == 0)
		unregister_chrdev_region(dev, num_of_dev);
	
	return -1;
}

static void mydev_exit(void)
{
	printk("[%s] \n", __func__);
    
	dev_t dev = MKDEV(major, 0);
    cdev_del(&mydev_cdev);
	device_destroy(mydev_class, dev);
	class_destroy(mydev_class);
    unregister_chrdev_region(dev, num_of_dev);
    printk(KERN_ALERT "%s driver removed.\n", DRIVER_NAME);

	//4. iounmap, if needed
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_LICENSE("GPL");


