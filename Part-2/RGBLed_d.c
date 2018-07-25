

//A Driver module to control the LED blinking sequence in the Galileo board using the gpio pins.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/ioctl.h>
#include <linux/gpio.h>
#include "led_ioctl.h"
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#define CONFIG 1
#define DEVICE_NAME "RGBLed"
#define cycle_time 20 // total cycle time is 20ms
#define step_duration 500// step time is 0.5s


struct myled{
	int dutycycle;
	int pin1;
	int pin2;
	int pin3;
} led;

static struct timer_list led_timer;
static int dutycycle;
static int npin1[3],npin2[3],npin3[3];
static int pin1,pin2,pin3;

struct RGBLed_dev {
	struct cdev cdev;          /* The cdev structure */
	char name[20];             /* Name of device*/
	char in_string[256];	   /* buffer for the input string */
	int current_write_pointer; /*current file pointer*/
} *RGB_ptr;


static struct class *RGBLed_class;          /* Class declaration for the device */
static struct device *RGBLed_device;        /* device declaration*/
static dev_t RGBLed_dnum;                   /* Allotted device number */
static int RGBLed_open(struct inode *, struct file *);
static int RGBLed_release(struct inode *, struct file *);
static ssize_t RGBLed_write(struct file *, const char *, size_t, loff_t *);
static int shortsleep_on;
static int shortsleep_off;
static int numiter;

int i,j;
int c[3];
int p[3];

static int time_calc_for_usleep(int,int);

static int time_calc_for_usleep(int dc, int on){ /*calculates the sleep duration based on given duty cycle*/
int temp2;
int temp1;

if(on ==1)
	`temp2 = dc;
else
    temp2 = 100-dc;

temp1 = ((temp2*cycle_time)/100);
return temp1;
}

static int* set_pin_values(int pin, int *npin) // assigning the values for gpio pins based on pin number
{

if (pin==0)
{
	npin[0]=11;
	npin[1]=32;
	npin[2]=0;
}
if (pin==1)
{
	npin[0]=12;
	npin[1]=28;
	npin[2]=45;
}
if (pin==2)
{
	npin[0]=13;
	npin[1]=34;
	npin[2]=77;
}
if (pin==3)
{
	npin[0]=14;
	npin[1]=16;
	npin[2]=76;
}
if (pin==4)
{
	npin[0]=6;
	npin[1]=36;
	npin[2]=0;
}
if (pin==5)
{
	npin[0]=0;
	npin[1]=18;
	npin[2]=66;
}
if (pin==6)
{
	npin[0]=1;
	npin[1]=20;
	npin[2]=68;
}
if (pin==7)
{
	npin[0]=38;
	npin[1]=0;
	npin[2]=0;
}
if (pin==8)
{
	npin[0]=40;
	npin[1]=0;
	npin[2]=0;
}
if (pin==9)
{
	npin[0]=4;
	npin[1]=22;
	npin[2]=70;
}
if (pin==10)
{
	npin[0]=10;
	npin[1]=26;
	npin[2]=74;
}
if (pin==11)
{
	npin[0]=5;
	npin[1]=24;
	npin[2]=44;
}
if (pin==12)
{
	npin[0]=15;
	npin[1]=42;
	npin[2]=44;
}
if (pin==13)
{
	npin[0]=7;
	npin[1]=30;
	npin[2]=46;
}
return npin;
}

static void ledtimer_cbfunction(unsigned long data) //timer callback fucntion
{

if(!(c[0]==0&&c[1]==0&&c[2]==0))
{
    gpio_set_value(npin1[0], 0);
    gpio_set_value(npin2[0], 0);
    gpio_set_value(npin3[0], 0);
	led_timer.expires += (shortsleep_on*HZ)/1000 ;
	add_timer(&led_timer);
}
else
{
    gpio_set_value(npin1[0], c[0]);
    gpio_set_value(npin2[0], c[1]);
    gpio_set_value(npin3[0], c[2]);
    led_timer.expires += (shortsleep_off*HZ)/1000 ;
	add_timer(&led_timer);
}
return;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{ 
	myled led;

	if (copy_from_user(&led, (myled *)arg, sizeof(myled)))
	{
	    return -EACCES;
	}
	dutycycle = led.dutycycle;
	pin1=led.pin1;
	pin2=led.pin2;
	pin3=led.pin3;

	printk(" \nYou have set Pin 1 at: %d ",pin1);
	printk(" \nYou have set Pin 2 at: %d ",pin2);
	printk(" \nYou have set Pin 3 at: %d ",pin3);
	set_pin_values(pin1,npin1);
	set_pin_values(pin2,npin2);
	set_pin_values(pin3,npin3);

	for(i=0;i<3;i++)
	{
		gpio_request(npin1[i], "sysfs");      // gpio is requested
		gpio_request(npin2[i], "sysfs");
		gpio_request(npin3[i], "sysfs");
		gpio_direction_output(npin1[i], 0);   // setting the gpio direction to "out"
		gpio_direction_output(npin2[i], 0);
		gpio_direction_output(npin3[i], 0);
		gpio_export(npin1[i], false);         // exporting pins to write the value
		gpio_export(npin2[i], false);
		gpio_export(npin3[i], false);

	}

	shortsleep_on = time_calc_for_usleep(dutycycle,1);// sleep time calculation based on duty cycle for the ON time
	shortsleep_off = time_calc_for_usleep(dutycycle,0);// sleep time calculation based on duty cycle for the OFF time
	numiter = step_duration/cycle_time;// calculation of number of iterations for a single step

	return 0;
}

// Open RGBLed driver
static int RGBLed_open(struct inode *inode, struct file *file)
{
	struct RGBLed_dev *RGB_ptr;
	/* Get the per-device structure that contains this cdev */
	RGB_ptr = container_of(inode->i_cdev, struct RGBLed_dev, cdev);
	/* Easy access to cmos_devp from rest of the entry points */
	file->private_data = RGB_ptr;
	printk("\n%s is openning \n", RGB_ptr->name);
	return 0;
}

// Release RGBLED driver
static int RGBLed_release(struct inode *inode, struct file *file)
{
	struct RGBLed_dev *RGB_ptr = file->private_data;
	printk("\n%s is closing\n", RGB_ptr->name);
	return 0;
}

// write function to write the value to the device file
ssize_t RGBLed_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{   
	static int seqnum;
	struct RGBLed_dev *RGB_ptr = file->private_data;
    c[1]=c[2]=c[0]=0;
	while (count) 
	{
		get_user(RGB_ptr->in_string[RGB_ptr->current_write_pointer], buf++); // reading the interger byte by byte
		count--;
		if(count)
		{
			RGB_ptr->current_write_pointer++;
			if( RGB_ptr->current_write_pointer == 256)
				RGB_ptr->current_write_pointer = 0;
		}
	}
    RGB_ptr->current_write_pointer=0; // setting the pointer back to the location to write new value
	printk("Writing -- %d %s \n", RGB_ptr->current_write_pointer, RGB_ptr->in_string);
	kstrtoint(RGB_ptr->in_string,10,&seqnum);
	printk("Temp string to int value = %d",seqnum);
    if(seqnum==0)
    {
        c[0]=0;
	    c[1]=0;
        c[2]=0;
	}
	else if(seqnum==1)
    {
        c[0]=0;
	    c[1]=0;
        c[2]=1;
	}
    else if(seqnum==2)
    {
        c[0]=0;
	    c[1]=1;
        c[2]=0;
	}
	else if(seqnum==3)
    {
	    c[0]=0;
		c[1]=1;
	    c[2]=1;
	}
	else if(seqnum==4)
    {
        c[0]=1;
	    c[1]=0;
        c[2]=0;
	}
  else if(seqnum==5)
    {
        c[0]=1;
	    c[1]=0;
        c[2]=1;
	}
  else if(seqnum==6)
    {
        c[0]=1;
	    c[1]=1;
        c[2]=0;
	}
	else if(seqnum==7)
    {
        c[0]=1;
	    c[1]=1;
        c[2]=1;
	}

  //printk("\n %d %d %d \n",c[0],c[1],c[2]);

 	if (c[0]==1)
    	gpio_set_value(npin1[0], 1);
 	if (c[1]==1)
		gpio_set_value(npin2[0], 1);
 	if (c[2]==1)
		gpio_set_value(npin3[0], 1);

	init_timer(&led_timer);
	led_timer.function = ledtimer_cbfunction;
	led_timer.expires += (shortsleep_on*HZ)/1000 ;
	led_timer.data = 0;
	add_timer(&led_timer);
	msleep(500);
	del_timer(&led_timer);

	return 0;
}

/* File operations structure. Defined in linux/fs.h */
static struct file_operations RGBLed_fops = {
    .owner		= THIS_MODULE,           /* Owner */
    .open		= RGBLed_open,       	 /* Open method */
    .release	= RGBLed_release,    	 /* Release method */
    .write		= RGBLed_write,      	 /* Write method */
    .unlocked_ioctl	= my_ioctl,    		 /*ioctl method */
};

/* Driver initialization */
 static int __init RGBLed_driver_init(void)
{
	int ret;
	/* Request dynamic allocation of a device major number */
	if (alloc_chrdev_region(&RGBLed_dnum, 0, 1, DEVICE_NAME) < 0)
	{
		printk(KERN_DEBUG "Can't register device\n"); return -1;
	}
	/* Populate sysfs entries */
	RGBLed_class = class_create(THIS_MODULE, DEVICE_NAME);
	/* Allocate memory for the per-device structure */
	RGB_ptr = kmalloc(sizeof(struct RGBLed_dev), GFP_KERNEL);

	if (!RGB_ptr) 
	{
		printk("Bad Kmalloc\n"); return -ENOMEM;
	}

	/* Request I/O region */
	sprintf(RGB_ptr->name, DEVICE_NAME);
	/* Connect the file operations with the cdev */
	cdev_init(&RGB_ptr->cdev, &RGBLed_fops);
	RGB_ptr->cdev.owner = THIS_MODULE;

	/* Connect the major/minor number to the cdev */
	ret = cdev_add(&RGB_ptr->cdev, (RGBLed_dnum), 1);

	if (ret)
	{
		printk("Bad cdev\n");
		return ret;
	}

	/* Send uevents to udev, so it'll create /dev nodes */
	RGBLed_device = device_create(RGBLed_class, NULL, MKDEV(MAJOR(RGBLed_dnum), 0), NULL, DEVICE_NAME);
	RGB_ptr->current_write_pointer = 0;
	printk("RGBLed driver initialized.\n");
	return 0;
}

/* Driver Exit function*/
void __exit RGBLed_driver_exit(void)
{
	/* Release the major number */
	unregister_chrdev_region((RGBLed_dnum), 1);
	/* Destroy device */
	device_destroy (RGBLed_class, MKDEV(MAJOR(RGBLed_dnum), 0));
	cdev_del(&RGB_ptr->cdev);
	kfree(RGB_ptr);
	/* Destroy driver_class */
	class_destroy(RGBLed_class);

	for(j=0;j<3;j++)
	{
	    gpio_unexport(npin1[j]);              
	    gpio_free(npin1[j]);                      
	    gpio_unexport(npin2[j]);
	    gpio_free(npin2[j]);
	    gpio_unexport(npin3[j]);
	    gpio_free(npin3[j]);
  	}
	printk("RGBLed driver removed.\n");
}

module_init(RGBLed_driver_init);
module_exit(RGBLed_driver_exit);
MODULE_LICENSE("GPL v2");
