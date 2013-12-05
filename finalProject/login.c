char *tty;

#include "ucode.c"

int readUntilChar(char * inbuff,char* outbuff,char seperator,int * offset);

main(int arc, char * argv[])
{
    char username[64],password[64];
    char buff[1024];
    char entry[64];
    int valid = 0,passfile = 0,offset = 0,gid,uid;
    tty = argv[1];
    close(0);close(1);close(2);
    open(tty,READ);//open read
    open(tty,WRITE);//open write
    open(tty,WRITE);//open ERR

    printf("ASLOGIN : opened %s as stdin,stdout,stderr\n",tty);
    signal(2,1); //this lets us ignore ctrl-c interrupts

    while(1)
    {
        printf("login: ");
        gets(username);
        printf("password: ");
        gets(password);
        //check user record
        //check password
        passfile = open("/etc/passwd",READ);
        printf("password file opened on FD %d\n",passfile);
        //printf("passfile contents:\n");
        read(passfile,buff,1024);
        //printf("%s",buff);
        //printf("-----------------------");
        printf("now checking against passwords\n");
        while(1)
        {
            if(readUntilChar(buff,entry,':',&offset)==-1)
            {
                break;
            }
            //printf("Entry read: %s, username: %s\n",entry,username);
            if(strcmp(entry,username)==0)
            {
                readUntilChar(buff,entry,':',&offset);
                //printf("Entry read: %s, username: %s\n",entry,password);
                if(strcmp(password,entry)==0)
                {
                    valid = 1;
                    break;
                }

            }
            else 
            {
                readUntilChar(buff,entry,'\n',&offset);
            }
            //printf("%s\n", entry);

        }

        if(valid)
        {
            printf("valid login\n logging you in now\n");

            //set uid
            //NOTE: this code only works for single digit UIDs and GIDs
            readUntilChar(buff,entry,':',&offset);
            gid = entry[0]-48; 
            readUntilChar(buff,entry,':',&offset);
            uid = entry[0]-48;
            chuid(uid,gid);
            
            readUntilChar(buff,entry,':',&offset);
            //change dir to user directrory
            readUntilChar(buff,entry,':',&offset);
            chdir(entry);
            //exec their shell program
            readUntilChar(buff,entry,'\n',&offset);
            printf("execing %s \n",entry);
            exec(entry);
        }
        else
        {
            offset = 0;
            printf("Login failed, please try again\n");
        }


    }

}
//reads characters from inbuff until the seperator character is read and puts
//them into outbuff, increments offset while doing so
//Returns: the number of characters read
int readUntilChar(char * inbuff,char * outbuff,char seperator,int * offset)
{
    int i = 0;
    while (inbuff[*offset] != seperator)
    {
        if (inbuff[*offset] == '\0' || (*offset)>=1024)
        {
            return -1; //if we hit end of buffer or a null terminator stop
        }
        outbuff[i] = inbuff[*offset];
        i++;
        (*offset)++;
    }
    (*offset)++; //skip the seperator
    outbuff[i] = '\0'; //add null terminator
    return i;
}
