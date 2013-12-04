char *tty;
#include "ucode.c"
main(int arc, char * argv[])
{
    char username[64],password[64];
    int valid = 0;
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
        if(valid)
        {
            //set uid
            //change dir to user directrory
            //exec their shell program

        }
        else
        {
            printf("Login failed, please try again\n");
        }


    }

}
