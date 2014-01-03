#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
//#include <linux/memory.h>

int hello_major = 0;
int hello_nums = 1;
module_param(hello_major, int, 0);
module_param(hello_nums, int, 1);

struct helloc_dev {
    struct list_head node;
    int num;
    char buf[100];
    struct cdev cdev;
};

dev_t dev;
LIST_HEAD(helloc_head);

static int hello_open(struct inode *node, struct file *filp)
{
    struct list_head *pos;
    struct helloc_dev *tmp;
    list_for_each(pos, &helloc_head) {
	tmp = container_of(pos, struct helloc_dev, node);
	if (iminor(node) == tmp->num) {
	    filp->private_data = tmp;
	    printk("helloc%d opened\n", tmp->num);
	    return 0;
	}
    }
    printk("helloc%d open failed\n", iminor(node));
    return -ENODEV;
}

static int hello_release(struct inode *node, struct file *filp)
{
    printk("hello released\n");
    return 0;
}

static ssize_t hello_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    struct helloc_dev *helloc = filp->private_data;    
    if(*ppos > sizeof(helloc->buf) - 1) {
	printk("helloc%d read count:%d\n", helloc->num, 0);
	return 0;
    }

    if(*ppos + count > sizeof(helloc->buf)) {
	count = sizeof(helloc->buf) - *ppos;
    }

    if (copy_to_user(buf, &helloc->buf[*ppos], count)) {
	return -EFAULT;
    }

    *ppos += count;
    printk("helloc%d read count:%d\n", helloc->num, count);
    return count;
}

static ssize_t hello_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    struct helloc_dev *helloc = filp->private_data;

    if(*ppos > sizeof(helloc->buf) - 1) {
	printk("helloc%d write failed,nomem\n", helloc->num);
	return -ENOMEM;
    }

    if(*ppos + count > sizeof(helloc->buf)) {
	count = sizeof(helloc->buf) - *ppos;
    }

    if (copy_from_user(&helloc->buf[*ppos], buf, count)) {
	return -EFAULT;
    }
    *ppos += count;
    printk("helloc%d write count:%d\n", helloc->num, count);
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
    int i;
    struct helloc_dev *tmp;
    printk("hello driver\n");
    if(hello_major) {
	dev = MKDEV(hello_major, 0);
	err = register_chrdev_region(dev, hello_nums, "helloc");
    } else {
	err = alloc_chrdev_region(&dev, 0, hello_nums, "helloc");
	hello_major = MAJOR(dev);
    }

    if(err) {
	printk("alloc_chrdev_region failed\n");
    } else {
	printk("major:%d minor:%d\n", MAJOR(dev), MINOR(dev));
	for(i = 0; i < hello_nums; i++) {
	    tmp = kzalloc(sizeof(struct helloc_dev), GFP_KERNEL);
	    if (tmp) {
		tmp->num = i;
		snprintf(&tmp->buf[0], sizeof(tmp->buf), "helloc%d", tmp->num);
		cdev_init(&tmp->cdev, &hello_fops);
		tmp->cdev.owner = THIS_MODULE;
		err = cdev_add(&tmp->cdev, MKDEV(hello_major, i), 1);
		if(err)
		    printk("cdev add failed,num:%d\n", i);
		list_add(&tmp->node, &helloc_head);
		printk("helloc%d added!\n", i);
	    } else {
		struct list_head *pos, *n;
		list_for_each_safe(pos, n, &helloc_head) {
		    tmp = container_of(pos, struct helloc_dev, node);
		    cdev_del(&tmp->cdev);
		    list_del(&tmp->node);
		    printk("helloc%d deleted!\n", tmp->num);
		    kfree(tmp);
		}
		goto err_alloc;
	    }
	}

    }
    return 0;
 err_alloc:
    unregister_chrdev_region(dev, hello_nums);
    return err;
}

void hello_exit(void)
{
    struct helloc_dev *tmp;
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &helloc_head) {
	tmp = container_of(pos, struct helloc_dev, node);
	cdev_del(&tmp->cdev);
	list_del(&tmp->node);
	printk("helloc%d deleted!\n", tmp->num);
	kfree(tmp);
    }

    unregister_chrdev_region(dev, hello_nums);
    printk("free dev:0x%08x ~0x%08x\n", dev, MKDEV(hello_major, hello_nums - 1));
    printk("say you, driver\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
