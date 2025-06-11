#include <linux/module.h>
#include <linux/init.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <linux/kernel.h>

#define SPI_BUS 0
#define SPI_BUS_CS1 0
#define SPI_BUS_SPEED 1000000

static struct spi_device *spi_device;

static int spi_device_probe(struct spi_device *spi)
{
    int ret;
    u8 tx[] = {0xAA};
    u8 rx[ARRAY_SIZE(tx)];

    spi->max_speed_hz = SPI_BUS_SPEED;
    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;
    ret = spi_setup(spi);
    if (ret) {
        pr_err("spi_setup failed\n");
        return ret;
    }

    ret = spi_write_then_read(spi, tx, ARRAY_SIZE(tx), rx, ARRAY_SIZE(rx));
    if (ret) {
        pr_err("spi_write_then_read failed\n");
        return ret;
    }

    pr_info("SPI device probed successfully\n");
    return 0;
}

static int spi_device_remove(struct spi_device *spi)
{
    pr_info("SPI device removed\n");
    return 0;
}

static const struct of_device_id spi_device_dt_ids[] = {
    { .compatible = "spi_device" },
    { }
};
MODULE_DEVICE_TABLE(of, spi_device_dt_ids);

static struct spi_driver spi_device_driver = {
    .driver = {
        .name = "spi_device",
        .of_match_table = spi_device_dt_ids,
    },
    .probe = spi_device_probe,
    .remove = spi_device_remove,
};

static int __init spi_device_init(void)
{
    int ret;
    struct spi_master *master;

    master = spi_busnum_to_master(SPI_BUS);
    if (!master) {
        pr_err("spi_busnum_to_master failed\n");
        return -ENODEV;
    }

    spi_device = spi_alloc_device(master);
    if (!spi_device) {
        pr_err("spi_alloc_device failed\n");
        return -ENOMEM;
    }

    spi_device->chip_select = SPI_BUS_CS1;
    spi_device->max_speed_hz = SPI_BUS_SPEED;
    spi_device->mode = SPI_MODE_0;
    spi_device->bits_per_word = 8;
    spi_device->dev.platform_data = NULL;
    strlcpy(spi_device->modalias, "spi_device", SPI_NAME_SIZE);

    ret = spi_add_device(spi_device);
    if (ret) {
        pr_err("spi_add_device failed\n");
        spi_dev_put(spi_device);
        return ret;
    }

    ret = spi_register_driver(&spi_device_driver);
    if (ret) {
        pr_err("spi_register_driver failed\n");
        spi_unregister_device(spi_device);
        return ret;
    }

    pr_info("SPI device driver registered\n");
    return 0;
}

static void __exit spi_device_exit(void)
{
    spi_unregister_driver(&spi_device_driver);
    spi_unregister_device(spi_device);
    pr_info("SPI device driver unregistered\n");
}

module_init(spi_device_init);
module_exit(spi_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BPB");
MODULE_DESCRIPTION("Simple SPI device driver example");
MODULE_VERSION("1.0");
