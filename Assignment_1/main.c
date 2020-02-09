#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                
#include<linux/uaccess.h>              
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/random.h>
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOW('a','b',int32_t*)

 
u_int32_t value,value1;
u_int16_t random;

u_int16_t random_number(void)
{
    uint16_t ran1;
    get_random_bytes(&ran1,2);
    return ran1;
}
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
 
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .unlocked_ioctl = etx_ioctl,
        .release        = etx_release
};
 
static int etx_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Opened \n");
        return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Device File Closed \n");
        return 0;
}
 
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Read Function\n");
        random=random_number()&0xff;
        printk(KERN_INFO "VALUE1= %d \n", value1);
        if(value1==0)
        {
        	random=random<<6;
        }
        printk(KERN_INFO "%d\n",random);
        copy_to_user(buf, &random, 2);
        return 0;
        //return random;
}
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        printk(KERN_INFO "Write function\n");
        return 0;
}
 
static long etx_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{ 
         switch(cmd) {
                case WR_VALUE:
                        copy_from_user(&value ,(int32_t*) arg, sizeof(value));
                        printk(KERN_INFO "You Entered channel no %d\n", value);
                        break;
                case RD_VALUE:
                        copy_from_user(&value1 ,(int32_t*) arg, sizeof(value1));
                        printk(KERN_INFO "You entered alignment as  %d\n", value1);
                     break;
        }
        return 0;
}
 
 
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/

        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/

        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/

        if((cdev_add(&etx_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"adc8")) == NULL){
            printk(KERN_INFO "Unable to create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "ADC Driver Inserted \n");
    return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
void __exit etx_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "ADC driver removed \n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KETAN NIRMAL");
MODULE_DESCRIPTION("ADC DEVICE DRIVER");

