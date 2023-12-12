#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//led_test /dev/xxx on
//led_test /dev/xxx off

int main(int argc, char **argv)
{
    int fd = 0;
    char status = 0;
    if(argc != 3)
    {
        printf("usage: led_test /dev/xxx on/off \n");
        return -1;
    }

    fd = open(argv[1], O_RDWR);
    if(fd == -1)
    {
        printf("oepn %s fail \n", argv[1]);
        return -1;
    }

    if(!strcmp(argv[2], "on"))
    {
        //turn on the led
        status = 1;
    }

    write(fd, &status, 1);

    close(fd);

    return 0;
}