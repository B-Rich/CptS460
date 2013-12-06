//reads characters from inbuff until the seperator character is read and puts
//them into outbuff, increments offset while doing so
//Returns: the number of characters read
int readUntilChar(char * inbuff,char * outbuff,char seperator,int * offset)
{
    int i = 0;
    while (inbuff[*offset] != seperator)
    {
        if (inbuff[*offset] == '\0')
        {
            //printf("hit this");
            return -1; //if we hit end of buffer or a null terminator stop
        }
        if((*offset)>=1024)
        {
            return i;
        }
        outbuff[i] = inbuff[*offset];
        i++;
        (*offset)++;
    }
    (*offset)++; //skip the seperator
    outbuff[i] = '\0'; //add null terminator
    return i;
}
