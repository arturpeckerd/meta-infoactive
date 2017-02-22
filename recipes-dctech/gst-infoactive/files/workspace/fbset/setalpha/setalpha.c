#include <stdio.h> 
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <linux/mxcfb.h>

int main(int argc, char **argv) 
{ 
    int fb_fd; 
    struct mxcfb_gbl_alpha gbl_alpha;  

    if(argc != 4){ 
        printf("Usage: %s alpha_val[0x0-0xFF] /dev/fbX\n enable[0-1]",argv[0]); 
        return -1; 
    }  

    if ((fb_fd = open(argv[2], O_RDWR, 0)) < 0){
        printf("Error in /dev/fbX \n");
        return -1;
    }

    gbl_alpha.alpha = strtol(argv[1], NULL, 16); 
    gbl_alpha.enable = atoi(argv[3]); 

    if (ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha) < 0) {
        printf("Error in applying Color Key\n");
    }

    close(fb_fd); 
    return 0; 
} 
