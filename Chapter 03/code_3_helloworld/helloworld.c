/*
 * The simplest hello-world kernel module
 */
#include <linux/init.h>   /* Needed for the macros */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info()  */

static int __init hello_init(void)
{
	pr_info("Hello, world\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Goodbye, world\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
