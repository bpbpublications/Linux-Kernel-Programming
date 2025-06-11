#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/io.h>

#define DRIVER_NAME "simple_pci"
#define PCI_VENDOR_ID_EXAMPLE 0x1234
#define PCI_DEVICE_ID_EXAMPLE 0x5678

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_EXAMPLE, PCI_DEVICE_ID_EXAMPLE), },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, pci_ids);

struct simple_pci_dev {
    struct pci_dev *pdev;
    void __iomem *mmio_base;
};

static int simple_pci_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    struct simple_pci_dev *dev;
    int bars, err;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;

    dev->pdev = pdev;
    pci_set_drvdata(pdev, dev);

    err = pci_enable_device(pdev);
    if (err)
        goto err_free_dev;

    bars = pci_select_bars(pdev, IORESOURCE_MEM);
    err = pci_request_selected_regions(pdev, bars, DRIVER_NAME);
    if (err)
        goto err_disable_device;

    dev->mmio_base = pci_iomap(pdev, 0, 0);
    if (!dev->mmio_base) {
        err = -EIO;
        goto err_release_regions;
    }

    pr_info(DRIVER_NAME ": device probed\n");
    return 0;

err_release_regions:
    pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
err_disable_device:
    pci_disable_device(pdev);
err_free_dev:
    kfree(dev);
    return err;
}

static void simple_pci_remove(struct pci_dev *pdev)
{
    struct simple_pci_dev *dev = pci_get_drvdata(pdev);

    pci_iounmap(pdev, dev->mmio_base);
    pci_release_selected_regions(pdev, pci_select_bars(pdev, IORESOURCE_MEM));
    pci_disable_device(pdev);
    kfree(dev);

    pr_info(DRIVER_NAME ": device removed\n");
}

static struct pci_driver simple_pci_driver = {
    .name = DRIVER_NAME,
    .id_table = pci_ids,
    .probe = simple_pci_probe,
    .remove = simple_pci_remove,
};

static int __init simple_pci_init(void)
{
    return pci_register_driver(&simple_pci_driver);
}

static void __exit simple_pci_exit(void)
{
    pci_unregister_driver(&simple_pci_driver);
}

module_init(simple_pci_init);
module_exit(simple_pci_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("Simple PCI driver example");
MODULE_VERSION("1.0");
