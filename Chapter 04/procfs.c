#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/minmax.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
#define HAVE_PROC_OPS
#endif


#define PROCFS_MAX_SIZE 2048UL
#define PROCFS_ENTRY_FILENAME "buffer"
#define PROCFS_ENTRY_FOLDER   "bpb"

static struct proc_dir_entry *parent;
static struct proc_dir_entry *proc_file;
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static ssize_t procfs_read(struct file *filp, char __user *buffer,
                           size_t length, loff_t *offset)
{
    if (*offset || procfs_buffer_size == 0) {
        pr_debug("procfs_read: END\n");
        *offset = 0;
        return 0;
    }
    procfs_buffer_size = min(procfs_buffer_size, length);
    if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
        return -EFAULT;
    *offset += procfs_buffer_size;

    pr_debug("procfs_read: read %lu bytes\n", procfs_buffer_size);
    return procfs_buffer_size;
}
static ssize_t procfs_write(struct file *file, const char __user *buffer,
                            size_t len, loff_t *off)
{
    procfs_buffer_size = min(PROCFS_MAX_SIZE, len);
    if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
        return -EFAULT;
    *off += procfs_buffer_size;

    pr_debug("procfs_write: write %lu bytes\n", procfs_buffer_size);
    return procfs_buffer_size;
}
static int procfs_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}
static int procfs_close(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

#ifdef HAVE_PROC_OPS
static struct proc_ops file_ops_proc_file = {
    .proc_read    = procfs_read,
    .proc_write   = procfs_write,
    .proc_open    = procfs_open,
    .proc_release = procfs_close,
};
#else
static const struct file_operations file_ops_proc_file = {
    .read    = procfs_read,
    .write   = procfs_write,
    .open    = procfs_open,
    .release = procfs_close,
};
#endif

static int __init procfs_init(void)
{
    parent = proc_mkdir(PROCFS_ENTRY_FOLDER,NULL);    
    if( parent == NULL )
    {
            pr_info("Error creating proc entry");
            goto r_device;
    }

    proc_file = proc_create(PROCFS_ENTRY_FILENAME, 0644, parent,
                                &file_ops_proc_file);
    if (proc_file == NULL) {
        pr_debug("Error: Could not initialize /proc/%s\n",
                 PROCFS_ENTRY_FILENAME);
        return -ENOMEM;
    }
    proc_set_size(proc_file, 80);
    proc_set_user(proc_file, GLOBAL_ROOT_UID, GLOBAL_ROOT_GID);

    pr_debug("/proc/%s created\n", PROCFS_ENTRY_FILENAME);
    return 0;
    
r_device:    
    return -1;
}

static void __exit procfs_exit(void)
{
    proc_remove(parent); // /proc/bpb
    // or 
    //remove_proc_entry("bpb/buffer", parent);
    
    pr_debug("/proc/%s removed\n", PROCFS_ENTRY_FILENAME);
}

module_init(procfs_init);
module_exit(procfs_exit);

MODULE_LICENSE("GPL");
