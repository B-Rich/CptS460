int pid, child, status,serial1=0,serial2=0;
int stdin,stdout;
#include "ucode.c"

int login();
int parent();

main(int arc, char * argv[])
{
    //open /dev/tty0 as read and write in order to display messages
    open("/dev/tty0",READ);//READ
    open("/dev/tty0",WRITE);//WRITE

    //once thats done we can use print f which calls putc() to write to stdout
    printf("ASINIT : fork a login task on console\n");
    child = fork();
    if (child)
        parent();
    else /* execute login task */
        login();



}

int login()
{
    exec("login /dev/tty0");
}

int parent()
{
    while(1)
    {
        
        //close(0);close(1);
        //start login procs on serial
        serial1=fork();
        if(serial1)
        {
            
        }
        else
        {
            exec("login /dev/ttyS0");
        }
        serial2=fork();
        if(serial2)
        {

        }
        else
        {
            exec("login /dev/ttyS1");
        }
        //open(0,READ);
        //open(1,WRITE);
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
