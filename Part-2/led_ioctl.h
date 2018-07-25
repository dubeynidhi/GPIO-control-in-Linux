#include <linux/ioctl.h>

typedef struct {
   int dutycycle;
   int pin1;
   int pin2;
   int pin3;
} myled;
#define CONFIG 1
