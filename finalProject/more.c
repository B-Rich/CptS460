#include "ucode.c"
#include "util.c"

int space(int fd, char* buff, char* line,int * offset);
int enter(int fd, char* buff, char* line, int* offset);



int main(int argc,char * argv[])
{
    int fd,offset=0,i,ret=0;
    char c;
    char buff[1024];
    char line[1024];
    if (argc < 2)
    {
        write(2,"usage: more [filename]",22);
        return -1;
    }
    fd = open(argv[1],READ);
    read(fd,buff,1024);
    for(i=0;i<24;i++)
    {
        readUntilChar(buff,line,'\n',&offset);
        printf("%s\r\n",line);
    }
    do
    {
        c = getc();
        if (c == ' ')
            ret = space(fd,buff,line,&offset);
        else if (c == '\r')
            ret = enter(fd,buff,line,&offset);
        //printf("ret: %d c: %c\r\n",ret,c);
    }while (c != 'q' && ret != -1);
    
}

//proccesses a press of the space bar
int space(int fd, char* buff, char* line,int * offset)
{
    int charsread,bytesread;
    //if it doesnt run out of space in the buffer
    charsread = readUntilChar(buff,line,'\n',offset);
    if ((*offset) == 1024)
    {
        *offset = 0;
        bytesread = read(fd,buff,1024);
        if (bytesread <1024)
        {
            buff[bytesread] = '\0';
        }
        charsread = readUntilChar(buff,line+charsread,'\n',offset);
        
    }
    if (charsread == -1)
    {
        //printf("this happened\r\n");
        return -1;
    }
    printf("%s\r\n",line);
    return 0;
}

//processes a press of the enter key
int enter(int fd, char* buff, char* line, int* offset)
{
    int i,j;
    for(i = 0;i<24;i++)
    {
        j=space(fd,buff,line,offset);
        if (j == -1) 
            return -1;
    }
    return 0;

}
