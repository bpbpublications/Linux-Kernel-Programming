#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *my_kthread;

int my_thread_function(void *data) {
    while (!kthread_should_stop()) {
        pr_info("Kernel Thread: Running\n");
        ssleep(5); // Sleep for 5 seconds
    }
    pr_info("Kernel Thread: Stopping\n");
    return 0;
}

static int __init my_module_init(void) {
    pr_info("Module Loaded\n");
    my_kthread = kthread_run(my_thread_function, NULL, "my_kthread");
    if (IS_ERR(my_kthread)) {
        pr_err("Failed to create kthread\n");
        return PTR_ERR(my_kthread);
    }
    return 0;
}

static void __exit my_module_exit(void) {
    kthread_stop(my_kthread);
    pr_info("Module Unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("A simple example of kthread usage");
