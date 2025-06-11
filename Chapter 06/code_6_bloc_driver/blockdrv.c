#include <linux/module.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>

#define BLOCK_SIZE 512
#define NUM_SECTORS 1024

static struct block_device_operations block_ops;

static struct gendisk *block_disk;
static struct request_queue *block_queue;

static unsigned char *block_memory;

static int block_major = 0;

static void block_transfer(struct request *req) {
    unsigned long offset = blk_rq_pos(req) * BLOCK_SIZE;
    unsigned long nbytes = blk_rq_bytes(req);

    if ((offset + nbytes) > (NUM_SECTORS * BLOCK_SIZE)) {
        printk(KERN_ERR "Block: Beyond-end write (%ld %ld)\n", offset, nbytes);
        return;
    }

    if (rq_data_dir(req) == WRITE) {
        memcpy(block_memory + offset, req->buffer, nbytes);
    } else {
        memcpy(req->buffer, block_memory + offset, nbytes);
    }
}

static int block_request(struct request_queue *q) {
    struct request *req;

    req = blk_fetch_request(q);
    while (req != NULL) {
        if (req == NULL || (req->cmd_type != REQ_TYPE_FS)) {
            printk(KERN_NOTICE "Block: Skip non-CMD request\n");
            __blk_end_request_all(req, -EIO);
            continue;
        }

        block_transfer(req);
        if (!__blk_end_request_cur(req, 0)) {
            req = blk_fetch_request(q);
        }
    }

    return 0;
}

static int __init block_init(void) {
    block_memory = kmalloc(NUM_SECTORS * BLOCK_SIZE, GFP_KERNEL);
    if (!block_memory) {
        return -ENOMEM;
    }

    block_queue = blk_init_queue(block_request, NULL);
    if (!block_queue) {
        kfree(block_memory);
        return -ENOMEM;
    }

    block_major = register_blkdev(0, "block");
    if (block_major < 0) {
        printk(KERN_WARNING "Block: Unable to get major number\n");
        return block_major;
    }

    block_disk = alloc_disk(1);
    if (!block_disk) {
        unregister_blkdev(block_major, "block");
        blk_cleanup_queue(block_queue);
        kfree(block_memory);
        return -ENOMEM;
    }

    block_disk->major = block_major;
    block_disk->first_minor = 0;
    block_disk->fops = &block_ops;
    block_disk->queue = block_queue;
    sprintf(block_disk->disk_name, "block");
    set_capacity(block_disk, NUM_SECTORS);
    add_disk(block_disk);

    return 0;
}

static void __exit block_exit(void) {
    del_gendisk(block_disk);
    put_disk(block_disk);
    unregister_blkdev(block_major, "block");
    blk_cleanup_queue(block_queue);
    kfree(block_memory);
}

module_init(block_init);
module_exit(block_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("bpb");
MODULE_DESCRIPTION("Simple Block Device Driver");

