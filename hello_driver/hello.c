#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

int __init hello_init(void)
{
    printk("hello driver\n");
    return 0;
}

void hello_exit(void)
{
    printk("say you, driver\n");
}

module_init(hello_init);
module_exit(hello_exit);

