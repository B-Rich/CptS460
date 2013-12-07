#include "ucode.c"

int main(int argc, char * argv[])
{
    int child=0;
    int pchild = 0;
    int pstatus = 0;
    int redirout = 0;
    int redirin = 0;
    int redirapp = 0;
    int redirpipe = 0;
    int the_pipe[2];
    int status;
    int pid;
    int numargs;
    int i,j;
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
        else if (strcmp(args[1],"exit")==0 || strcmp(args[1],"logout")==0)
        {
            exit();
        }
        else
        {

            //identify if there are any pipes or redirections
            //handle pipes
            //handle redirection

            for (i = 1;i<numargs;i++)
            {
                if (strcmp(args[i],">")==0)
                {
                    //flag to redirect out
                    redirout = 1;
                    //copy the args before args[i] into args[0]
                    strcpy(args[0],"");
                    for (j = 1;j<i;j++)
                    {
                        strcat(args[0],args[j]);
                        strcat(args[0]," ");
                    }
                    strcpy(args[1],args[i+1]);
                    break;
                    
                }
                else if (strcmp(args[i],"<")==0)
                {
                    //flag to redirect in
                    redirin = 1; 
                    //copy the args before args[i] into args[0]
                    strcpy(args[0],"");
                    for (j = 1;j<i;j++)
                    {
                        strcat(args[0],args[j]);
                        strcat(args[0]," ");
                    }
                    strcpy(args[1],args[i+1]);
                    break;
                }
                else if (strcmp(args[i],">>")==0)
                {
                    //flag to redirect out
                    redirapp = 1;
                    //copy the args before args[i] into args[0]
                    strcpy(args[0],"");
                    for (j = 1;j<i;j++)
                    {
                        strcat(args[0],args[j]);
                        strcat(args[0]," ");
                    }
                    strcpy(args[1],args[i+1]);
                    break;


                }
                else if (strcmp(args[i],"|")==0)
                {
                    redirpipe = 1;
                     //copy the args before args[i] into args[0]
                    strcpy(args[0],"");
                    for (j = 1;j<i;j++)
                    {
                        strcat(args[0],args[j]);
                        strcat(args[0]," ");
                    }
                    //copy the args after the pipe into args[1]
                    strcpy(args[1],"");
                    for (j = i+1;j<numargs;j++)
                    {
                        strcat(args[1],args[j]);
                        strcat(args[1]," ");
                    }
                    break;

                }
            }


            child = fork();
            if (child)
            {
                redirout = 0;
                redirin = 0;
                redirapp = 0;
                redirpipe= 0;
                pid = wait(&status);
            }
            else
            {
                if (redirpipe)
                {
                    pipe(the_pipe);
                    pchild = fork();
                    if (pchild)
                    {
                        //parent is the read end of the pipe
                        close(0);
                        open(the_pipe[0],READ);
                        redirpipe = 0;
                        //wait(&pstatus);
                        status = exec(args[1]);
                    }
                    else
                    {
                        //child is the write end of the pipe
                        close(1);
                        open(the_pipe[1],WRITE);
                        redirpipe = 0;
                        pstatus = exec(args[1]);
                    }
                }
                else if (redirout)
                {
                    close(1);
                    open(args[1],O_WRONLY | O_CREAT);
                    redirout = 0;
                    status = exec(args[0]);
                }
                else if (redirapp)
                {
                    close(1);
                    open(args[1],O_WRONLY | O_CREAT | O_APPEND);
                    redirapp = 0;
                    status = exec(args[0]);
                }
                else if (redirin)
                {
                    close(0);
                    open(args[1],READ);
                    redirin = 0;
                    status = exec(args[0]);
                }
                else
                {
                    status = exec(args[0]);
                }
            }
        }
    }
}
