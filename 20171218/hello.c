#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>

MODULE_LICENSE("Dual BSD/GPL");

int x;
char* chr;
module_param(x, int, S_IRUGO);

static int hello_init(void)
{
	int i = 0;
	for(i = 0;i < x;i++)
	{
		printk(KERN_ALERT "hello world!\n");
	}
	//printk(KERN_ALERT "hello world!\n");
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);
