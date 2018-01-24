#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/mm.h>
#include <linux/kdev_t.h>
#include <asm/page.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

#include <linux/device.h>

static int simple_major = 0;
static struct cdev SimpleDev;



module_param(simple_major, int, 0);
MODULE_AUTHOR("Jonathan Corbet");
MODULE_LICENSE("Dual BSD/GPL");

/*
 * Open the device; in fact, there's nothing to do here.
 */
  int simple_open(struct inode *inode, struct file *filp)
{
	printk("open!!!!!!!!!!");
	return 0;
}


/*
 * Closing is just as simpler.
 */
static int simple_release(struct inode *inode, struct file *filp)
{
	printk("close!!!!!!!!!!!\n");
	return 0;
}

static ssize_t simple_read(struct file *filep, char __user *buf, size_t len, loff_t *pos)  
{  
	char temp[64];
	if(len>64)  
	{  
		len =64;  
	}  
	if(copy_to_user(buf,temp,len))  
	{  
		return -EFAULT;  
	}     
	return len;  
}



  
static ssize_t simple_write(struct file *filep, const char __user *buf, size_t len, loff_t *pos)  
{  
	char temp[64];
	if(len>64)  
	{  
		len = 64;  
	}  
	if(copy_from_user(temp,buf,len))  
	{  
		return -EFAULT;  
	}  
	printk("write %s\n",temp);  
	return len;  
}  



/*
 * Our various sub-devices.
 */
/* Device 0 uses remap_pfn_range */
static struct file_operations simple_ops = {
	.owner   = THIS_MODULE,
	.open    = simple_open,
	.release = simple_release,
	.read    = simple_read,
	.write   = simple_write,
};


/*
 * Set up the cdev structure for a device.
 */
static void simple_setup_cdev(struct cdev *dev, int minor,
		struct file_operations *fops)
{
	int err, devno = MKDEV(simple_major, minor);
    
	cdev_init(dev, fops);
	dev->owner = THIS_MODULE;
	dev->ops = fops;
	err = cdev_add (dev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk (KERN_NOTICE "Error %d adding simple%d", err, minor);
}






/*
 * We export two simple devices.  There's no need for us to maintain any
 * special housekeeping info, so we just deal with raw cdevs.
 */


/*
 * Module housekeeping.
 */
static int simple_init(void)
{
	int result;
	dev_t dev = MKDEV(simple_major, 0);
	/* Figure out our device number. */
	if (simple_major)
		result = register_chrdev_region(dev, 1, "simple");
	else {
		result = alloc_chrdev_region(&dev, 0, 1, "simple");
		simple_major = MAJOR(dev);
	}
	if (result < 0) {
		printk(KERN_WARNING "simple: unable to get major %d\n", simple_major);
		return result;
	}
	if (simple_major == 0)
		simple_major = result;

	/* Now set up 1 cdevs. */
	simple_setup_cdev(&SimpleDev, 0, &simple_ops);
	printk("driver inin........... \n");
	return 0;
}





static void simple_cleanup(void)
{
	cdev_del(&SimpleDev);
	unregister_chrdev_region(MKDEV(simple_major, 0), 1);
	printk("driver exit........... \n");
}


module_init(simple_init);
module_exit(simple_cleanup);
