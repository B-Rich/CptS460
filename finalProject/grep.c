#include "ucode.c"
#include "util.c"
int grep(char * pattern, char * line);

int main(int argc,char * argv[])
{
    int fd,offset=0,bytesread,readchar;
    char buff[1024];
    char line[1024];
    if(argc < 3)
    {
        fd = 0;
    }
    else
    {
        fd = open(argv[2],READ);
    }

    //get lines from file, print them if they match pattern
    //no regex support required(thank god)
    
    bytesread = read(fd,buff,1024);
    //printf("fd = %d,buff = %s,bytes read = %d\r\n",fd,buff,bytesread);

    do 
    {
        //printf("\r\nBefore: buff: %s\r\n line: %s\r\n offset: %d\r\n",buff,line,offset);
        readchar = readUntilChar(buff,line,'\n',&offset);
        //printf("Chars read %d",readchar);
        if (offset == 1024)
        {
            //we have a split line to handle
            offset = 0;
            bytesread = read(fd,buff,1024);
            if (bytesread < 1024)
            {
                buff[bytesread] = '\0';
            }
            readchar = readUntilChar(buff,line+readchar,'\n',&offset);
        }
        if (grep(argv[1],line)==1)
        {
            printf("%s\r\n",line);
        }
    }while(readchar != -1 );
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
