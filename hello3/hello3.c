#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/cdev.h>

dev_t dev;
struct cdev *g_cdev;

static int hello_open(struct inode *node, struct file *filp)
{
    printk("hello opened\n");
    return 0;
}

static int hello_release(struct inode *node, struct file *filp)
{
    printk("hello released\n");
    return 0;
}

static ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    printk("hello read\n");
    return 0;
}

static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    printk("hello write\n");
    return count;
}

static const struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .write = hello_write,
};
int __init hello_init(void)
{
    int err;
    printk("hello driver\n");
    err = alloc_chrdev_region(&dev, 0, 1, "hello3");
    //dev = MKDEV(250, 3);
    //err = register_chrdev_region(dev, 1, "hello2_name");
    if(err) {
	printk("alloc_chrdev_region failed\n");
    } else {
	printk("major:%d minor:%d\n", MAJOR(dev), MINOR(dev));
	g_cdev = cdev_alloc();
	if (g_cdev) {
	    printk("cdev alloc successed\n");
	    g_cdev->owner = THIS_MODULE;
	    g_cdev->ops = &hello_fops;
	    if(cdev_add(g_cdev, dev, 1)) {
		printk("cdev add failed\n");
	    }
	}
    }
    return 0;
}

void hello_exit(void)
{
    if(g_cdev) {
	cdev_del(g_cdev);
    }
    unregister_chrdev_region(dev, 1);
    printk("free dev:0x%08x\n", dev);
    printk("say you, driver\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
