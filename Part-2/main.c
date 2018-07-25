
#include <stdio.h>
#include <unistd.h>
#include <linux/input.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sched.h>
#include <pthread.h>
#include <poll.h>
#include <linux/ioctl.h>
#include "led_ioctl.h"
#define CONFIG 1

#define MOUSEFILE "/dev/input/event2"
#define RGBLed "/dev/RGBLed"

int flag=0; // flag to determine if the mouse was clicked
sem_t m;
myled led;
int npin1[3],npin2[3],npin3[3];
char str[3];


/*LED blinking sequence consisting 7 steps of single, double and triple LEDs*/
void* start_led_sequence(void* args)
{

    int fd1,fd2;
    int seq;
    fd1 = open(RGBLed, O_RDWR);
    if (fd1< 0)
    {
		printf("Can not open device file.\n");
		return 0;
	}
    fd2=ioctl(fd1,1,&led);
    if (fd2< 0)
    {
		printf("Can not open ioctl.\n");
		return 0;
	}
	/*semaphore for locking*/
	sem_wait(&m);


	while(1) // indefinite loop for LED blinking sequence
	{
    	if(flag==1)
    	{
        	break; // exit the loop if flag is set to 1 by the mouse click
  		}

		if(flag!=1)
		{
			seq=1 ;// for Blue
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=2 ;// for Green
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=3 ;// for Green Blue
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=4 ;// for Red
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=5 ;// for Red Blue
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=6; // for REd Green
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}

		if(flag!=1)
		{
			seq=7 ;// for Red Green Blue
			printf("\n printing!! seq %d",seq);
			sprintf(str,"%d",seq);
			write(fd1,str, 4);
			printf("\n printing!! %s",str);
		}
	}

	return 0;
}
//Main function to get the user inputs and create thread 
int main(){


	sem_init (&m,0,0);
    printf("\n Specify the percentage of duty cycle : ");
    scanf("%d",&led.dutycycle); 
    printf("\n Enter pin number for Red light : ");
    scanf("%d",&led.pin1);
    printf("\n Enter pin number for Green light : ");
    scanf("%d",&led.pin2);
    printf("\n Enter pin number for Blue light : ");
    scanf("%d",&led.pin3);

    /*Validating the entered pin numbers*/
    if(led.pin1<0 || led.pin1>13 || led.pin2<0 || led.pin2>13 || led.pin3<0 || led.pin3>13 || led.pin1==7 || led.pin1==8 ||led.pin2==7 || led.pin2==8||led.pin3==7 || led.pin3==8)
	{
	    printf("The given pin number is wrong\n");
	    printf("The program is exiting ...\n");
	    exit(1); 
    }
    /*Validating the entered dutyclycle*/
    if( led.dutycycle >100 || led.dutycycle < 0){
	    printf("The given dutycycle is wrong\n");
	    printf("The program is exiting ...\n");
	    exit(1); 
    }

	int fd;
	struct input_event ie;
	fd = open(MOUSEFILE, 0);

	pthread_t thread;
	pthread_create (&thread, NULL, &start_led_sequence, NULL);

	sem_post(&m);
	while(1) /* While loop to monitor mouse click event from the user*/
		{
		    read(fd, &ie, sizeof(struct input_event));
		    if((ie.code == 272 && ie.value == 1)||(ie.code == 273 && ie.value == 1))
		{
			printf("\nclick detected");
			flag=1;
			break;
		}
		else
		{
			sem_post(&m);
		}
	}
	printf("\nexit loop");
	pthread_join(thread,NULL);
  	close(fd);
	return 0;
}
