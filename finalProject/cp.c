#include "ucode.c"

int main (int argc, char * argv[])
{
    int infd,outfd,bytesread;
    char buff[1024];
    if (argc != 3)
    {
        //write error to stderr
        write(2,"Error: cp must have 2 arguments\nUsage cp f1 f2\n",48);
        
    }
    else
    {
        infd = open(argv[1],READ);
        printf("argv[1] = %s,fd = %d",argv[1],infd);
        outfd = open(argv[2],WRITE);
        if (outfd == -1)
        {
            creat(argv[2]);
            outfd = open(argv[2],WRITE);
        }
        printf("argv[2] = %s,fd = %d",argv[2],outfd);
        do
        {
            bytesread = read(infd,buff,1024);
            write(outfd,buff,bytesread);
        }while(bytesread == 1024);
        close(infd);
        close(outfd);

    }

}
