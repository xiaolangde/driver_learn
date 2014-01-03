#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

dev_t dev;
int __init hello_init(void)
{
    int err;
    printk("hello driver\n");
    dev = MKDEV(250, 3);
    //err = alloc_chrdev_region(&dev, 0, 1, "hello1");
    err = register_chrdev_region(dev, 1, "hello2_name");
    if(err) {
	printk("alloc_chrdev_region failed\n");
    } else {
	printk("major:%d minor:%d\n", MAJOR(dev), MINOR(dev));
    }
    return 0;
}

void hello_exit(void)
{
    unregister_chrdev_region(dev, 1);
    printk("free dev:0x%08x\n", dev);
    printk("say you, driver\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("Dual BSD/GPL");
