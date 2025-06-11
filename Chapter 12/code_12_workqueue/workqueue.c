#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/workqueue.h>

static struct workqueue_struct *my_wq;
static struct work_struct my_work;

void my_work_function(struct work_struct *work) {
    pr_info("Workqueue: Executing my work function\n");
}

static int __init my_module_init(void) {
    pr_info("Module Loaded\n");

    // Create a custom workqueue
    my_wq = create_workqueue("my_workqueue");
    if (!my_wq) {
        pr_err("Failed to create workqueue\n");
        return -ENOMEM;
    }

    // Initialize and queue the work item
    INIT_WORK(&my_work, my_work_function);
    queue_work(my_wq, &my_work);

    return 0;
}

static void __exit my_module_exit(void) {
    // Destroy the workqueue
    destroy_workqueue(my_wq);
    pr_info("Module Unloaded\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("A simple example of workqueue usage");
