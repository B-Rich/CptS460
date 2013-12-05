#include "ucode.c"
#include "util.c"
int grep(char * pattern, char * line);

int main(int argc,char * argv[])
{
    int fd,offset;
    char buff[1024];
    char line[1024];
    if(argc > 3)
    {
        fd = 0;
    }
    else
    {
        fd = open(argv[2],READ);
    }
    
    //get lines from file, print them if they match pattern
    //no regex support required(thank god)
    read(fd,buff,1024);
    while (readUntilChar(buff,line,'\n',offset) != -1)
    {
        if (grep(argv[1],line)==1)
        {
            printf("%s\r\n",line);
        }
    }
    
}


//returns true if the pattern is in the line, or false otherwise
int grep(char * pattern, char * line)
{
    int pindex=0,lindex=0;
    //printf("\r\nDebug output: line = %s, pattern = %s\r\n",line,pattern);
    while (line[lindex]!='\r' && line[lindex]!='\n')
    {
        if (pindex == strlen(pattern))
        {
            return 1;
        }
        else if (line[lindex]==pattern[pindex])
        {
            //printf("happened: line = %c pattern = %c\r\n",line[lindex], pattern[pindex]);
            pindex ++;
        }
        else
        {
            pindex = 0;
        }
        lindex++;
    }
    return 0;
}
