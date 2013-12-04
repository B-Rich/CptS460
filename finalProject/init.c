int pid, child, status;
int stdin,stdout;
#include "ucode.c"
main(int arc, char * argv[])
{
    //open /dev/tty0 as read and write in order to display messages
    

    //once thats done we can use print f which calls putc() to write to stdout
    printf("ASINIT : fork a login task on console\n");
    child = fork();
    if (child)
        parent();
    else /* execute login task */
        login();



}

int login
{
    exec("login /dev/tty0");
}

int parent()
{
    while(1)
    {
        
        printf("ASINIT : waiting ..... \n");
        pid = wait(&status);
        if (pid == child)
        {
            //fork another login child 
            printf("ASINIT : fork a login task on console\n");
            child = fork();
            if (child)
                parent();
            else /* execute login task */
                login();
        }
        else
            printf("INIT: buried an orphan child %d\n", pid);
    }

}
