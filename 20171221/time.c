#include<linux/init.h>
#include<linux/module.h>
#include<linux/jiffies.h>
#include<linux/delay.h>

MODULE_LICENSE("GPL");

static int time_init(void)
{
	int i, t1, t2;
	printk("init........");
	for(i = 0; i < 10; i++)
	{	t1 = jiffies;		
		ssleep(1);
	//	ssleep(1);
		t2 = jiffies;
		printk("%lo\n",(long)(t2-t1)/HZ);
	}
	return 0;
}

static void time_exit(void)
{
	printk("exit........");
}

module_init(time_init);
module_exit(time_exit);

