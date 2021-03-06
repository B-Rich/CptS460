// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kmode", "switch", "wait", "exit","fork",
"exec",0};

int show_menu()
{
   printf("********************** Menu *************************\n");
   printf("* ps  chname  kmode  switch  wait  exit  fork  exec *\n");
   printf("*****************************************************\n");
}

int find_cmd(name) char *name;
{
   int i=0;   
   char *p=cmd[0];

   while (p){
         if (strcmp(p, name)==0)
            return i;
         i++;  
         p = cmd[i];
   } 
   return(-1);
}

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   syscall(1, 0, 0);
}

int chname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0);
}

int kmode()
{
    printf("kmode : syscall #3 to enter Kmode\n");
    printf("proc %d going K mode ....\n", getpid());
        syscall(3, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}    

int kswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        syscall(4,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int exit()
{
   int exitValue;
   printf("\nenter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(exitValue) int exitValue;
{
  syscall(6,exitValue,0);
}

int ufork()
{
    int child;
    printf("\nForking a new proc");
    child = syscall(7,0,0);
    printf("\nchild spawned with PID %d\n",child);
    return child;

}

int uexec()
{
    char s[64];
    int child;
    printf("input path to image");
    gets(s);
    child = syscall(8,s,0);
    printf("\nchild spawned with PID %d\n",child);
    return child;


}

int getc()
{
  return syscall(90,0,0) & 0x7F;
}

int putc(c) char c;
{
  return syscall(91,c,0,0);
}


int invalid(name) char *name;
{
    printf("Invalid command : %s\n", name);
}
