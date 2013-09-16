/************ t.c file **********************************/
#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD      0                /* proc status     */
#define READY     1      

typedef struct proc{
    struct proc *next;   
    int  ksp;               /* saved sp; offset = 2 */
    int  pid;
    int  ppid;              //the parent's PID
    int  status;            /* READY|DEAD, etc */
    int  kstack[SSIZE];     // kmode stack of task
    int  priority;          //priority of process
}PROC;

//#include "io.c"  /* <===== use YOUR OWN io.c with printf() ****/

PROC proc[NPROC], *running, *thequeue[NPROC];

int  procSize = sizeof(PROC);

/****************************************************************
  Initialize the proc's as shown:
  running ---> proc[0] -> proc[1];

  proc[1] to proc[N-1] form a circular list:

  proc[1] --> proc[2] ... --> proc[NPROC-1] -->
  ^                                         |
  |<---------------------------------------<-

  Each proc's kstack contains:
  retPC, ax, bx, cx, dx, bp, si, di, flag;  all 2 bytes
 *****************************************************************/

int body();  
PROC * getproc();
void dequeue(PROC ** queue);
void enqueue(PROC *p, PROC **queue);

int initialize()
{
    int i, j;
    PROC *p;

    for (i=0; i < NPROC; i++){
        p = &proc[i];
        p->next = &proc[i+1];
        p->pid = i;
        p->status = READY;

        if (i){     // initialize kstack[ ] of proc[1] to proc[N-1]
            for (j=1; j<10; j++)
                p->kstack[SSIZE - j] = 0;          // all saved registers = 0
            p->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
            p->ksp = &(p->kstack[SSIZE-9]);        // ksp -> kstack top
        }
    }
    running = &proc[0];
    proc[NPROC-1].next = &proc[1];
    tswitch();   /* journey of no return */        
}




char *gasp[NPROC]={
    "Oh! You are killing me .......",
    "Oh! I am dying ...............", 
    "Oh! I am a goner .............", 
    "Bye! Bye! World...............",      
};

int grave(){
    printf("\n*****************************************\n"); 
    printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
    printf("*****************************************\n");
    running->status = DEAD;

    tswitch();   /* journey of no return */        
}

int ps()
{
    PROC *p;

    printf("running = %d\n", running->pid);

    p = running;
    p = p->next;
    printf("readyProcs = ");
    while(p != running && p->status==READY){
        printf("%d -> ", p->pid);
        p = p->next;
    }
    printf("\n");
}

int body()
{  char c;
    while(1){
        ps();
        printf("I am Proc %d in body()\n", running->pid);
        printf("Input a char : [s|q|f] ");
        c=getc();
        switch(c){
            case 's': tswitch(); break;
            case 'q': grave();   break;
            case 'f': kfork();   break;
            default :            break;  
        }
    }
}


int main()
{
    printf("\nWelcome to the 460 Multitasking System\n");
    initialize();
    printf("P0 switch to P1\n");
    tswitch();
    printf("P0 resumes: all dead, happy ending\n");
}


int scheduler()
{
    PROC *p;
    p = running->next;

    while (p->status != READY && p != running)
        p = p->next;

    if (p == running)
        running = &proc[0];
    else
        running = p;

    printf("\n-----------------------------\n");
    printf("next running proc = %d\n", running->pid);
    printf("-----------------------------\n");
}

int kfork()
{
    PROC* p = getproc();
    int j,i=0;
    if (p){
        while (&proc[i]!=p)//find what the PID of this proc is supposed to be
            i++;
        p->pid = i;
        p->status = READY;
        for (j=1; j<10; j++)
            p->kstack[SSIZE - j] = 0;          // all saved registers = 0
        p->kstack[SSIZE-1]=(int)body;          // called tswitch() from body
        p->ksp = &(p->kstack[SSIZE-9]);        // ksp -> kstack top
        p->ppid = running->pid;

        printf("\n*****************************************\n"); 
        printf("Forked a new task with PID: %d and parent: %d\n",p->pid,p->ppid);
        printf("*****************************************\n");


        return i;
    }
    else
    {
        return -1;
    }


}

PROC * getproc()
{
    PROC *p;
    int i;
    for (i = 0; i<NPROC;i++)
    {
        p = &proc[i];
        if (p->status == DEAD)
            return p;
    }
    return 0;

}

void enqueue(PROC *p, PROC **queue)
{
    int i=0;
    PROC * tmp;
    PROC * tmp2;
    //add the new proc pointer to the end of the array
    while(queue[i]!=0)
    {
        if (p->priority > queue[i]->priority)
        {
            //shuffle everything down one and stick p in the current place
            tmp = queue[i];
            queue[i]=p;
            i++;
            for (i;i<NPROC;i++)
            {
                tmp2 = queue[i];
                queue[i] = tmp;
                tmp = tmp2;
                if (tmp = 0)
                {
                    break;//break out if we are at the empty section of queue
                }
            }
        }
        else
        {
            i++;
        }

    }
    queue[i] = p;
}

void dequeue(PROC ** queue)
{
    int i = 0;
    while((queue[i]!=0)&&(i+1<NPROC))
    {
        queue[i]=queue[i+1];
        i++;
    }

}






