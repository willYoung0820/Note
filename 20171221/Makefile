#Makefile 2.6  
      
obj-m :=time.o  
KERNEL :=/usr/src/linux-headers-4.10.0-42-generic/  
PWD :=$(shell pwd)  
modules :  
	$(MAKE) -C $(KERNEL) M=$(PWD) modules  
.PHONEY:clean  
clean :  
	rm -f *.o *.ko  
