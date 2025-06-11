#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/security.h>
#include <linux/fs.h>

// Hook function for file permissions
static int my_lsm_inode_permission(struct inode *inode, int mask) {
    // Check if the current process has permission to access the file
    if (!security_task_is_allowed(current, mask)) {
        printk(KERN_INFO "LSM: Access denied for PID %d to inode %lu\n",
               current->pid, inode->i_ino);
        return -EACCES; // Access denied
    }
    return 0; // Access granted
}

// Hook function for file creation
static int my_lsm_inode_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl) {
    // Check if the current process is allowed to create a file in the directory
    if (!security_task_is_allowed(current, MAY_WRITE)) {
        printk(KERN_INFO "LSM: Creation of file denied for PID %d in directory %lu\n",
               current->pid, dir->i_ino);
        return -EACCES; // Creation denied
    }
    return 0; // Creation allowed
}

// Hook function for file deletion
static int my_lsm_inode_unlink(struct inode *dir, struct dentry *dentry) {
    // Check if the current process is allowed to delete the file
    if (!security_task_is_allowed(current, MAY_WRITE)) {
        printk(KERN_INFO "LSM: Deletion of file denied for PID %d in directory %lu\n",
               current->pid, dir->i_ino);
        return -EACCES; // Deletion denied
    }
    return 0; // Deletion allowed
}

// Hook functions registration
static struct security_hook_list my_lsm_hooks[] = {
    LSM_HOOK_INIT(inode_permission, my_lsm_inode_permission),
    LSM_HOOK_INIT(inode_create, my_lsm_inode_create),
    LSM_HOOK_INIT(inode_unlink, my_lsm_inode_unlink),
    // Add more hooks as needed
};

// LSM initialization
static int __init my_lsm_init(void) {
    printk(KERN_INFO "LSM: Initializing LSM\n");
    // Register the LSM hooks
    security_add_hooks(my_lsm_hooks, ARRAY_SIZE(my_lsm_hooks));
    return 0;
}

// LSM cleanup
static void __exit my_lsm_exit(void) {
    printk(KERN_INFO "LSM: Cleaning up LSM\n");
}

module_init(my_lsm_init);
module_exit(my_lsm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("A simple Linux Security Module");
