#include "ucode.c"

int main(int argc, char *argv[])
{
    int fd,bytesread;
    char buff[1024];
    if (argc == 1)
    {
        fd = 0;
    }
    else
    {
        fd = open(argv[1],READ);
    }
    //printf("argv[1] = %s,opened on fd %d",argv[1],fd);

    do
    {
        bytesread = read(fd,buff,1024);
        write(1,buff,bytesread);
    }while(bytesread == 1024);
    close(fd);



}
