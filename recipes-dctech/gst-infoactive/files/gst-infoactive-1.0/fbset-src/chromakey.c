#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/mxcfb.h>


int main (int argc, char *argv[])
{
    int fd_fb;
    struct mxcfb_color_key color_key;

    if(argc != 4){
        printf("Usage: %s color_key[0x0-0xFF] /dev/fbX\n enable[0-1]",argv[0]);
        return -1;
    }

    //color_key.color_key = 0x00FF0000;
    color_key.color_key = strtol(argv[1], NULL, 16);
    color_key.enable = atoi(argv[3]);

    if ((fd_fb = open(argv[2], O_RDWR, 0)) < 0){
        printf("Error in /dev/fbX \n");
        return -1;
    }

    if ( ioctl(fd_fb, MXCFB_SET_CLR_KEY, &color_key) < 0) {
        printf("Error in applying Color Key\n");
    }

    close (fd_fb);
 
    return 0;
}
