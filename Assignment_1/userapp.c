
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOW('a','b',int32_t*)
//#define RIGHT_ALLIGNED _IOR('a','b',int32_t*)
 
int main()
{
        int fd;
        u_int32_t x, number;
        u_int16_t value;
        
 
        printf("\nOpening Driver\n");
        fd = open("/dev/adc8", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
 
        printf("Enter the channel number of adc from 1 to 8\n");
        scanf("%d",&number);
        if(0<number<9)
        {        
        printf("Writing Value to Driver\n");
        ioctl(fd, WR_VALUE, (int32_t*) &number); 
        }
        
          printf("Enter 0 for allignment at higher bytes and 1 for  lower alignment \n");
          scanf("%d",&x);
          if(x==0 || x==1)
          {
          ioctl(fd, RD_VALUE, (int32_t*) &x);         
          read(fd,&value,2);
          printf("Value of adc from selected channel is %d\n", value);
         }

        
         
         else
         {
          printf("wrong allignment chossen \n");      
         }
        

        printf("Closing Driver\n");
        close(fd);
}
