#include <linux/module.h> // thu vien nay dinh nghia cac macro nhu module_init va module_exit
#include <linux/fs.h>	// thu vien nay dinh nghia cac ham allocate major & minor number

#define DRIVER_AUTHOR "Trung xxxxxxxxx@gmail.com"
#define DRIVER_DESC "hello world kernel module"
#define DRIVER_VERSION "1.0"


/* constructor */
static int __init hello_world_init(void)
{
	printk(KERN_INFO "hello world kernel module\n");
	return 0;
}

/* destructor */
static void __exit hello_world_exit(void)
{
	printk(KERN_INFO "Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION); 	