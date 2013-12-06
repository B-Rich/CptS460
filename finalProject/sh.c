#include "ucode.c"

int main(int argc, char * argv[])
{
    int child=0;
    int status;
    int pid;
    int numargs;
    int i;
    char line[64];
    char * tok;
    char args[8][64];
    while(1)
    {

        printf("alexSH: ");
        gets(line);
        //tokenize the string
        strcpy(args[0],line);
        tok = strtok(line," ");
        numargs = 1;
        while (tok != 0)
        {
            strcpy(args[numargs],tok);
            tok = strtok(0, " ");
            numargs++;
        }

        //print out arg list for debugging
        for (i = 0;i<numargs;i++)
        {
            printf("args[%d] = %s\r\n",i,args[i]);
        }


        //check if it is a change dir symbol
        if (strcmp(args[1],"cd")==0)
        {
            if (strcmp(args[2],"")==0)
            {
                //try to go to home directory?
            }
            else
                chdir(args[2]);
        }
        else if (strcmp(args[1],"exit")==0)
        {
            exit();
        }
        else
        {

            //identify if there are any pipes or redirections
            //handle pipes
            //handle redirection



            child = fork();
            if (child)
            {
                pid = wait(&status);
            }
            else
            {
                status = exec(args[0]);
            }
        }
    }
}
