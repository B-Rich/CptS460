#include "ucode.c"
#include "util.c"

void space(int fd, char* buff, char* line,int * offset);


int main(int argc,char * argv[])
{
    int fd,offset=0,i;
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
            space(fd,buff,line,&offset);
    }while (c != 'q');
    
}

void space(int fd, char* buff, char* line,int * offset)
{
    int charsread;
    //if it doesnt run out of space in the buffer
    charsread = readUntilChar(buff,line,'\n',offset);
    printf("%s\r\n",line);

}
