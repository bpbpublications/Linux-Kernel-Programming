#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <asm/uaccess.h>

MODULE_AUTHOR("TG");
MODULE_DESCRIPTION("Panic Button driver: no URB + char device");
MODULE_LICENSE("GPL");

#define VENDOR_ID	0x1130
#define PRODUCT_ID	0x0202

// Private structure
struct usb_panicb {
	struct usb_device *	udev;
	unsigned int		button;
};

// Forward declaration 
static struct usb_driver panicb_driver;

/* Table of devices that work with this driver */
static struct usb_device_id id_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{ },
};
MODULE_DEVICE_TABLE (usb, id_table);

/* Ask panic button for button status */
static int get_panicb_button_status (struct usb_panicb *panicb_dev)
{
  char *buf;
  int ret = 0;

  printk (KERN_INFO "get_panicb_button_status\n");

  // Allocate msg buffer
  if (!(buf = kmalloc(8, GFP_KERNEL))) {
    printk(KERN_WARNING "panicb: can't alloc buf\n");
    return -1;
  }

  memset (buf, 0, 8);	
  ret = usb_control_msg (panicb_dev->udev, usb_rcvctrlpipe (panicb_dev->udev, 0), 0x01, 0xA1, 0x300, 0x00, buf, 8, 2 * HZ);
  if (ret < 0) 
    printk (KERN_WARNING "panicb: IN, ret = %d\n", ret);
  else
    panicb_dev->button = *buf;
    
  
  kfree (buf);

  return 0;
}

// Char device functions

static int panicb_open (struct inode *inode, struct file *file)
{
  struct usb_panicb *dev;
  struct usb_interface *interface;
  int minor;
  
  minor = iminor(inode);

  // Get interface for device
  interface = usb_find_interface (&panicb_driver, minor);
  if (!interface)
    return -ENODEV;

  // Get private data from interface
  dev = usb_get_intfdata (interface);
  if (dev == NULL) {
      printk (KERN_WARNING "panicb: can't find device for minor %d\n", minor);
      return -ENODEV;
  }

  // Set to file structure
  file->private_data = (void *)dev;

  return 0;
}

static int panicb_release (struct inode *inode, struct file *file)
{
  return 0;
}

static int panicb_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  struct usb_panicb *dev;

  printk(KERN_DEBUG "panicb_ioctl\n");

  /* get the dev object */
  dev = file->private_data;
  if (dev == NULL)
    return -ENODEV;

  switch (cmd) {
    case 0 :
      printk(KERN_INFO "panicb_ioctl\n");
      if (get_panicb_button_status (dev) == 0) {
	if (copy_to_user((void*)arg, &(dev->button), sizeof(int*))) {
	  printk (KERN_WARNING "panicb: copy_to_user error\n");
	  return -EFAULT;
	}
      }

      break;

    default :
      printk(KERN_WARNING "panicb_ioctl(): unsupported command %d\n", cmd);
      
      return -EINVAL;
  }

  return 0;
}


static struct file_operations panicb_fops = {
  .open    = panicb_open,
  .release = panicb_release,
  .ioctl   = panicb_ioctl
};

// USB driver functions

static struct usb_class_driver panicb_class_driver = {
  .name = "usb/panicb",
  .fops = &panicb_fops,
  .minor_base = 0
};

static int panicb_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
  struct usb_device *udev = interface_to_usbdev (interface);
  struct usb_panicb *panicb_dev;
  int ret;

  printk (KERN_INFO "panicb_probe: starting\n");

  ret = usb_register_dev(interface, &panicb_class_driver);
  if (ret < 0) {
    printk (KERN_WARNING "panicb: usb_register_dev() error\n");
    return ret;
  }

  panicb_dev = kmalloc (sizeof(struct usb_panicb), GFP_KERNEL);
  if (panicb_dev == NULL) {
    dev_err (&interface->dev, "Out of memory\n");
    return -ENOMEM;
  }

  // Fill private structure and save it with usb_set_intfdata
  memset (panicb_dev, 0x00, sizeof (*panicb_dev));
  panicb_dev->udev = usb_get_dev(udev);
  panicb_dev->button = 0;
  usb_set_intfdata (interface, panicb_dev);

  dev_info(&interface->dev, "USB Panic Button device now attached\n");

  return 0;
}

static void panicb_disconnect(struct usb_interface *interface)
{
  struct usb_panicb *dev;

  dev = usb_get_intfdata (interface);
  usb_deregister_dev (interface, &panicb_class_driver);
  usb_set_intfdata (interface, NULL);
  kfree(dev);

  dev_info(&interface->dev, "USB Panic Button now disconnected\n");
}

static struct usb_driver panicb_driver = {
	.name       =	"panicb",
	.probe      =	panicb_probe,
	.disconnect =	panicb_disconnect,
	.id_table   =	id_table,
};

// Init & exit

static int __init usb_panicb_init(void)
{
  int retval = 0;

  retval = usb_register(&panicb_driver);
  if (retval)
    printk(KERN_WARNING "usb_register failed. Error number %d", retval);

  return retval;
}

static void __exit usb_panicb_exit(void)
{
  usb_deregister(&panicb_driver);
}

module_init (usb_panicb_init);
module_exit (usb_panicb_exit);
