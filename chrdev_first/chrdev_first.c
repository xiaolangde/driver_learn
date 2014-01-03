#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>	
//#include <linux/proc_fs.h>
//#include <linux/fcntl.h>	/* O_ACCMODE */
//#include <linux/aio.h>
//#include <asm/uaccess.h>

//#include <linux/ioctl.h>
#include <linux/cdev.h>

struct one_dev {
    struct cdev cdev;
    
};

struct one_dev *g_one_dev;
dev_t g_major;

static int one_open(struct inode *inode, struct file *filp)
{
    struct one_dev *dev;
    dev = container_of(inode->i_cdev, struct one_dev, cdev);
    filp->private_data = dev;
    return 0;
}

static ssize_t one_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_ops)
{
    struct one_dev *dev;
    dev = filp->private_data;
    printk("write data: %s", buf);
    return count;
}

struct file_operations one_fops = {
    .owner = THIS_MODULE,
    .open = one_open,
    .write = one_write,
};

static void one_dev_setup(struct one_dev *dev)
{
    int err;
    cdev_init(&dev->cdev, &one_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &one_fops;
    err = cdev_add(&dev->cdev, g_major, 1);
    if(err)
	printk(KERN_NOTICE "Error %d adding one_dev\n", err);
}

int one_init(void)
{
    int err;
    err = alloc_chrdev_region(&g_major, 0, 1, "one_dev");
    //if(unlikely(IS_ERR(err))) {
    if(err) {
	printk("alloc chrdev failed!\n");
    } else {
	printk("alloc chrdev success:major=%d,minor=%d\n",
	       MAJOR(g_major), MINOR(g_major));
	g_major = MAJOR(g_major);
	g_one_dev = kmalloc(sizeof(*g_one_dev), GFP_KERNEL);
	if(NULL == g_one_dev) {
	    printk("alloc mem failed\n");
	    err = -ENOMEM;
	    goto free_chrdev;
	}
	one_dev_setup(g_one_dev);
    }
    printk("one dev add success\n");
    printk("g_major = %d\n", g_major);
    printk("kk parent of cdev is %d\n", g_one_dev->cdev.kobj.parent);
    return 0;
 free_chrdev:
    unregister_chrdev_region(g_major, 1);
    return err;
}

void one_exit(void)
{
    cdev_del(&g_one_dev->cdev);
    kfree(g_one_dev);
    printk("g_major = %d\n", g_major);
    unregister_chrdev_region(g_major, 1);
    printk("kk one dev exited\n");
}

module_init(one_init);
module_exit(one_exit);

MODULE_AUTHOR("zhangyunrui");
MODULE_LICENSE("Dual BSD/GPL");
