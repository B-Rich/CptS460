/************ t.c file **********************************/
/*
 *Author: Alex Schuldberg
 *Date: September 16th 2013
 *Description: A simple OS program that is capable of forking
 * generic processes and implements a priority queue 
 */





#define NPROC     9        
#define SSIZE  1024                /* kstack int size */

#define DEAD      0                /* proc status     */
#define READY     1      
#define FREE      0
#define ZOMBIE    2
#define SLEEP     3

typedef struct proc{
    struct proc *next;   
    int  ksp;               /* saved sp; offset = 2 */
    int  pid;
    int  ppid;              //the parent's PID
    int  status;            /* READY|DEAD, etc */
    int  kstack[SSIZE];     // kmode stack of task
    int  priority;//priority of process
    int  exitVal;
    int  event;
}PROC;


PROC proc[NPROC], *running, *readyqueue[NPROC];

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
void printQueue(PROC ** queue);
int sleep(int event);
void wakeup(int event);
int wait( int * status);
void doWake();
int doSleep();

int initialize()
{
    int i;
    PROC *p;
    for (i =0;i<NPROC;i++)
        readyqueue[i]=0;

    p = &proc[0];
    p->pid = 0;
    p->status = READY;
    p->priority = 0;

    running = &proc[0];
    tswitch();   /* journey of no return */        
}




char *gasp[NPROC]={
    "Oh! You are killing me .......",
    "Oh! I am dying ...............", 
    "Oh! I am a goner .............", 
    "Bye! Bye! World...............",      
};
//kills a process
int grave(){
    int i,parent;
    if (running->pid == 1)
    {
        for (i = 2;i<NPROC;i++)
        {
            if ((&proc[i])->status != DEAD)
                return;
        }


    }
    if (running->pid != 1)
    {
        running->exitVal = 0;
        for (i = 2;i<NPROC;i++)
        {
            if ((&proc[i])->ppid == running->pid)
                (&proc[i])->ppid = 1;
        }
    }
    running->status = ZOMBIE;
    parent = running->ppid;
    wakeup((int)(&proc[parent]));
    printf("\n*****************************************\n"); 
    printf("Task %d %s\n", running->pid,gasp[(running->pid) % 4]);
    printf("*****************************************\n");

    tswitch();   /* journey of no return */        
}

int ps()
{
    PROC *p;

    printf("running = %d\n", running->pid);

    p = running;
    p = p->next;
    printf("readyProcs = ");
    printQueue(readyqueue);  
    printf("\n");
}

int body()
{  char c;
    while(1){
        ps();
        printf("I am Proc %d in body()\n", running->pid);
        printf("Input a char : [[s]witch|[k]ill|[f]ork|[w]ait|w[a]ke|s[l]eep] ");
        c=getc();
        switch(c){
            case 's': tswitch(); break;
            case 'k': grave();   break;
            case 'f': kfork();   break;
            case 'w': wait();    break;
            case 'a': doWake();  break;
            case 'l': doSleep(); break;
            default :            break;  
        }
    }
}

void doWake()
{
    char c;
    printf("\nenter an event to wake\n");
    c = getc();
    return wakeup(c-'0');
}
int doSleep()
{
    char c;
    printf("\nenter an event to sleep on\n");
    c = getc();
    return sleep(c-'0');

}





int main()
{
    printf("\nWelcome to the 460 Multitasking System\n");
    initialize();
    kfork();
    printf("P0 switch to P1\n");
    tswitch();
    printf("P0 resumes: all dead, happy ending\n");
}


int scheduler()
{
    PROC *p;
    if (running->status == READY)
        enqueue(running,readyqueue);
    running = readyqueue[0];
    dequeue(readyqueue);

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
        p->priority = 1;
        enqueue(p,readyqueue);

        printf("\n*****************************************\n"); 
        printf("Forked a new task with PID: %d and parent: %d\n and address %d\n",p->pid,p->ppid,p);
        printf("*****************************************\n");


        return i;
    }
    else
    {
        return -1;
    }


}
// gets an available PID and proc from the list of procs
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

// enters a proc into the queue specified according to it's priority
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
//dequeues the end of the queue
//note does not return a proc, get that with queue[0]
void dequeue(PROC ** queue)
{
    int i = 0;
    while((queue[i]!=0)&&(i+1<NPROC))
    {
        queue[i]=queue[i+1];
        i++;
    }
    queue[i] = 0; //zero out last element in the list
}

//prints the queue in tasteful arrow notation
void printQueue(PROC ** queue)
{
    int i=0;
    while (queue[i]!=0)
    {
        printf("%d <- ",queue[i]->pid);
        i++;
    }
    printf("start");
}

//tells the proc to wait until one of it's children becomes zombified
//childless processes are not alowed to wait
int wait( int * status)
{
    int i,childPid;
    //first we find the child
    for (i = 2;i<NPROC;i++)
    {
        if ((&proc[i])->ppid == running->pid)
        {
            childPid = i;
            break;
        }
    }
    if (i==NPROC)
    {
        return -1;
    }
    while(1)
    {

        if((&proc[childPid])->status == ZOMBIE)
        {
            *status = (&proc[childPid])->exitVal;
            (&proc[childPid])->status = FREE;
            return childPid;
        }
        sleep(running);
    }
}
//makes a proc sleep until woken by the specified event
int sleep(int event)
{
    running->event = event;
    running->status = SLEEP;
    printf("sleeping on event: %d",event);
    tswitch();
}
//wakes all the procs up that are sleeping on the specified event
void wakeup(int event)
{
    int i;
    printf("waking on event: %d",event);
    for (i = 0;i<NPROC;i++)
    {
        if ((&proc[i])->event == event)
        {
            (&proc[i])->status = READY;
            enqueue(&proc[i],readyqueue);
        }

    }
}




