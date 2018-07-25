
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

#define MOUSEFILE "/dev/input/event2"
#define cycle_time 20000 // total cycle time is 20ms
#define step_duration 500000// step time is 0.5s

int npin1[3],npin2[3],npin3[3],dutycycle,fo[9];
int flag=0; // flag to determine if the mouse was clicked
sem_t m;

int time_calc_for_usleep(int,int);

/*calculates the sleep duration based on given duty cycle*/
int time_calc_for_usleep(int dc, int on)
{ 
  int temp;
  if(on ==1)
    temp = dc/100;
  else
    temp = 1-(dc/100);

    temp = temp*cycle_time;
  return temp;
}

/*assigning the values for gpio pins based on pin number */
int* set_pin_values(int pin, int *npin) 
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

/* Exporting the gpio pin*/
void gp_export(int gpioport)
{
  FILE *fp = fopen("/sys/class/gpio/export","w");
  fprintf(fp,"%d",gpioport);
  fclose(fp);
}

/* Unexporting the gpio pin*/
void gp_unexport(int gpioport)
{
  FILE *fp = fopen("/sys/class/gpio/unexport","w");
  fprintf(fp,"%d",gpioport);
  fclose(fp);
}

/* Setting the pin as output(direction)*/
void init_gpio(int gpioport)
{
  int fd;
	char filename[256];
	sprintf(filename,"/sys/class/gpio/gpio%d/direction",gpioport);
	fd = open(filename, O_WRONLY);
  write(fd,"out",3);
  close(fd);
}
/*Opening the file*/
int setopen(int gpioport)
{
  int fd=0;
  char filename[256];
  sprintf(filename,"/sys/class/gpio/gpio%d/value",gpioport);
  fd = open(filename,O_WRONLY);
  if (fd < 0)
    {
         printf("\n gpio%d value open failed",gpioport);
    }
  return fd;
}

/*Setting the value to the port*/
void setval(int port,int val)
{
if(val==0)
		write(port,"0",1);
if(val==1)
		write(port,"1",1);
}

/*LED blinking sequence consisting 7 steps of single, double and triple LEDs*/
void* start_led_sequence(void* args)
{
  sem_wait(&m);//semaphore for locking
  int i;
  int shortsleep_on = time_calc_for_usleep(dutycycle,1);// sleep time calculation based on duty cycle for the ON time
  int shortsleep_off = time_calc_for_usleep(dutycycle,0);// sleep time calculation based on duty cycle for the OFF time
  int numiter = step_duration/cycle_time;// calculation of number of iterations for a single step

	while(1) // indefinite loop for LED blinking sequence
	{
    if(flag==1)
    {
      break; // exit the loop if flag is set to 1 by the mouse click
    }

  	if(flag!=1)
    {
  		for(i=0;i<numiter;i++){
		    setval(fo[0],1);
		    usleep(shortsleep_on);
		    setval(fo[0],0);
		    usleep(shortsleep_off);
        }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[3],1);
		    usleep(shortsleep_on);
		    setval(fo[3],0);
		    usleep(shortsleep_off);
      }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[6],1);
		    usleep(shortsleep_on);
		    setval(fo[6],0);
		    usleep(shortsleep_off);
      }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[0],1);
		    setval(fo[3],1);
		    usleep(shortsleep_on);
		    setval(fo[0],0);
		    setval(fo[3],0);
		    usleep(shortsleep_off);
      }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[3],1);
		    setval(fo[6],1);
		    usleep(shortsleep_on);
		    setval(fo[3],0);
		    setval(fo[6],0);
		    usleep(shortsleep_off);
      }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[0],1);
		    setval(fo[6],1);
		    usleep(shortsleep_on);
		    setval(fo[0],0);
		    setval(fo[6],0);
		    usleep(shortsleep_off);
      }
    }
    if(flag!=1)
    {
      for(i=0;i<numiter;i++){
		    setval(fo[0],1);
		    setval(fo[3],1);
		    setval(fo[6],1);
		    usleep(shortsleep_on);
		    setval(fo[0],0);
		    setval(fo[3],0);
		    setval(fo[6],0);
		    usleep(shortsleep_off);
      }
    }
  }
return 0;
}

int main()
{

int p1,p2,p3,i;
sem_init (&m,0,0);
printf("\n Specify the percentage of duty cycle : ");
scanf("%d",&dutycycle); //getting the dutycycle value from the user
printf("\n Enter pin number 1 : ");
scanf("%d",&p1);// getting the pin number for the first LED
set_pin_values(p1,npin1);
printf("\n Enter pin number 2 : ");
scanf("%d",&p2);// getting the pin number for the second LED
set_pin_values(p2,npin2);
printf("\n Enter pin number 3 : ");
scanf("%d",&p3);// getting the pin number for the third LED
set_pin_values(p3,npin3);

/*Validating the entered pin numbers*/
if(p1<0 || p1>13 || p2<0 || p2>13 || p3<0 || p3>13)
{
  printf("The given pin number is wrong\n");
  printf("The program is exiting ...\n");
  exit(1); //exit if the entered pin number is wrong
}
/*Validating the entered dutyclycle*/
if( dutycycle >100 || dutycycle < 0)
{
  printf("The given dutycycle is wrong\n");
  printf("The program is exiting ...\n");
  exit(1); //exit if the enterd duty cycle is incorrect
}

gp_export(npin1[0]);
gp_export(npin1[1]);
gp_export(npin1[2]);

init_gpio(npin1[0]);
init_gpio(npin1[1]);
init_gpio(npin1[2]);

gp_export(npin2[0]);
gp_export(npin2[1]);
gp_export(npin2[2]);

init_gpio(npin2[0]);
init_gpio(npin2[1]);
init_gpio(npin2[2]);

gp_export(npin3[0]);
gp_export(npin3[1]);
gp_export(npin3[2]);

init_gpio(npin3[0]);
init_gpio(npin3[1]);
init_gpio(npin3[2]);

fo[0]=setopen(npin1[0]);
fo[1]=setopen(npin1[1]);
fo[2]=setopen(npin1[2]);

fo[3]=setopen(npin2[0]);
fo[4]=setopen(npin2[1]);
fo[5]=setopen(npin2[2]);

fo[6]=setopen(npin3[0]);
fo[7]=setopen(npin3[1]);
fo[8]=setopen(npin3[2]);


setval(fo[1],0);
setval(fo[2],0);
setval(fo[4],0);
setval(fo[5],0);
setval(fo[7],0);
setval(fo[8],0);

sem_init (&m,0,0);
int fd,j;
struct input_event ie;
fd = open(MOUSEFILE, 0);

pthread_t thread;
pthread_create (&thread, NULL, &start_led_sequence, NULL);

sem_post(&m);
while(1)
	{
    read(fd, &ie, sizeof(struct input_event));
    if((ie.code == 272 && ie.value == 1)||(ie.code == 273 && ie.value == 1))
    	{
    		printf("\nclick detected");
    		flag=1;
    		break; // exit of left click
    	}
  	else
    	{
    		sem_post(&m);
    	}
  }
printf("\nexit loop");
pthread_join(thread,NULL);
close(fd);
for(i=0;i<9;i++)
{
  close(fo[i]);
}

for(j=0;j<3;j++)
{
  gp_unexport(npin1[j]);
  gp_unexport(npin2[j]);
  gp_unexport(npin3[j]);
}

return 0;
}
