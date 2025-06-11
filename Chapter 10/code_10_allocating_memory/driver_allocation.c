#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mm.h>

#define DRIVER_NAME "memory_allocation_driver"

struct mem_alloc_driver {
    void *kmalloc_buffer;
    void *vmalloc_buffer;
    struct page *alloc_pages_buffer;
    unsigned long get_free_pages_buffer;
    struct dma_pool *dma_pool_buffer; // We'll declare this but won't use it since it's not part of this example
};

static int __init mem_alloc_init(void)
{
    struct mem_alloc_driver *drv;

    drv = kmalloc(sizeof(struct mem_alloc_driver), GFP_KERNEL);
    if (!drv)
        return -ENOMEM;

    drv->kmalloc_buffer = kmalloc(1024, GFP_KERNEL);
    if (!drv->kmalloc_buffer)
        goto fail;

    drv->vmalloc_buffer = vmalloc(4096);
    if (!drv->vmalloc_buffer)
        goto fail_kmalloc;

    drv->alloc_pages_buffer = alloc_pages(GFP_KERNEL, 2); // Allocate 2 pages (8KB)
    if (!drv->alloc_pages_buffer)
        goto fail_vmalloc;

    drv->get_free_pages_buffer = __get_free_pages(GFP_KERNEL, 2); // Allocate 2 pages (8KB)
    if (!drv->get_free_pages_buffer)
        goto fail_alloc_pages;

    // DMA pool initialization (not used in this example)
    // drv->dma_pool_buffer = dma_pool_create("example_pool", NULL, 4096, 32, 0);

    pr_info("Memory allocation example driver loaded successfully\n");
    return 0;

fail_alloc_pages:
    __free_pages(drv->alloc_pages_buffer, 2);
fail_vmalloc:
    vfree(drv->vmalloc_buffer);
fail_kmalloc:
    kfree(drv->kmalloc_buffer);
fail:
    kfree(drv);
    return -ENOMEM;
}

static void __exit mem_alloc_exit(void)
{
    struct mem_alloc_driver *drv = NULL;

    // Cleanup resources
    // DMA pool cleanup (not used in this example)
    // if (drv->dma_pool_buffer)
    //     dma_pool_destroy(drv->dma_pool_buffer);

    // Free resources
    if (drv) {
        __free_pages(drv->alloc_pages_buffer, 2);
        vfree(drv->vmalloc_buffer);
        kfree(drv->kmalloc_buffer);
        kfree(drv);
    }

    pr_info("Memory allocation example driver unloaded\n");
}

module_init(mem_alloc_init);
module_exit(mem_alloc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("Memory Allocation Example Driver");
