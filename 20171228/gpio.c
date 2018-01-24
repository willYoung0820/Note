#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/gpio.h>
#include <mach/gpio-bank-n.h>

#define DEVICE_NAME     "key_device"
#define DEVICE_MAJOR    240

void key_tasklet_func(void);

DECLARE_TASKLET(key_tasklet,key_tasklet_func,0);

struct key_irq_desc
{
    int irq;
    int pin;
    int pin_setting;
    int number;
    char *name;

};

static struct key_irq_desc key_irqs [] =
{
    {IRQ_EINT(0), S3C64XX_GPN(0) ,  S3C64XX_GPN0_EINT0 , 0, "KEY0"},
    {IRQ_EINT(1), S3C64XX_GPN(1) ,  S3C64XX_GPN1_EINT1 , 1, "KEY1"},
    {IRQ_EINT(2), S3C64XX_GPN(2) ,  S3C64XX_GPN2_EINT2 , 2, "KEY2"},
    {IRQ_EINT(3), S3C64XX_GPN(3) ,  S3C64XX_GPN3_EINT3 , 3, "KEY3"},
    {IRQ_EINT(4), S3C64XX_GPN(4) ,  S3C64XX_GPN4_EINT4 , 4, "KEY4"},
    {IRQ_EINT(5), S3C64XX_GPN(5),   S3C64XX_GPN5_EINT5 , 5, "KEY5"},
};
static volatile int key_values [] = {1, 1, 1, 1, 1, 1};

//初始化等待列对
static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int event_flag = 0;


static irqreturn_t buttons_interrupt(int irq, void *dev_id)
{
    struct key_irq_desc *dev_irqs = (struct key_irq_desc *)dev_id;
    int down;

    down = gpio_get_value(dev_irqs->pin);

    //按键发生了变化
    if (down != (key_values[dev_irqs->number] & 1))
    {
        // Changed

        key_values[dev_irqs->number] = 0 + down;

        event_flag = 1;
        //唤醒列对
        wake_up_interruptible(&button_waitq);
    }

    tasklet_schedule(&key_tasklet);

    return IRQ_RETVAL(IRQ_HANDLED);
}

void key_tasklet_func(void)
{
    printk("<0>=======task========\n");
    printk("<0>task function test~\n");
    printk("<0>===================\n");
}

static int s3c6410_key_open(struct inode *inode, struct file *file)
{
    int i;
    int err = 0;

    //注册中断              结构体未注册完
    for (i = 0; i < sizeof(key_irqs)/sizeof(key_irqs[0]); i++)
    {
        if (key_irqs[i].irq < 0)
        {
            continue;
        }

        //                    中断号                 中断处理寒酸     中断处理属性
        err = request_irq(key_irqs[i].irq, buttons_interrupt, IRQ_TYPE_EDGE_BOTH,
                          key_irqs[i].name, (void *)&key_irqs[i]);
        if (err)
            break;
    }

    //如果注册中断过程中出错，则注销注册成功的中断
    if (err)
    {
        i--;
        for (; i >= 0; i--)
        {
            if (key_irqs[i].irq < 0)
            {
                continue;
            }
            disable_irq(key_irqs[i].irq);
            free_irq(key_irqs[i].irq, (void *)&key_irqs[i]);
        }
        return -EBUSY;
    }


    //显示一次主界面而已
    event_flag = 1;

    return 0;
}


static int s3c6410_keys_close(struct inode *inode, struct file *file)
{
    int i;
    //注销中断
    for (i = 0; i < sizeof(key_irqs)/sizeof(key_irqs[0]); i++)
    {
        if (key_irqs[i].irq < 0)
        {
            continue;
        }
        free_irq(key_irqs[i].irq, (void *)&key_irqs[i]);
    }

    return 0;
}


static int s3c6410_keys_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
    unsigned long err;

    if (!event_flag)
    {
        //没有新按键事件处理，如果用户要求不阻塞就直接返回，否则阻塞在button_waitq队列上。
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;
        else
            wait_event_interruptible(button_waitq, event_flag);
    }

    event_flag = 0;

    err = copy_to_user(buff, (const void *)key_values, min(sizeof(key_values), count));

    return err ? -EFAULT : min(sizeof(key_values), count);
}



static struct file_operations key_device_fops =
{
    .owner      =   THIS_MODULE,
    .open       =   s3c6410_key_open,
    .release    =   s3c6410_keys_close,
    .read       =   s3c6410_keys_read,
};



static int __init key_device_init(void)
{
    int ret;
    printk ("<0>key init\n");
    //注册字符设备
    ret = register_chrdev(DEVICE_MAJOR,DEVICE_NAME,&key_device_fops);
    if (ret <0)
    {
        printk ("<0>register %s char dev error\n","key");
        return -1;
    }
    printk ("<0>success\n");
    return 0;
}

static void __exit key_device_exit(void)
{
    //注销设备
    unregister_chrdev(DEVICE_MAJOR,DEVICE_NAME);
    printk ("<0>module exit\n");
}

module_init(key_device_init);
module_exit(key_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("hebaichuan");