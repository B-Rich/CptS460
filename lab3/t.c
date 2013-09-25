/* Alex Schuldberg
 * 11143076
 * CPTS 460
 * LAB 3
 */


#include "type.h"

/********* in type.h ***********
  typedef unsigned char   u8;
  typedef unsigned short u16;
  typedef unsigned long  u32;

#define NULL     0
#define NPROC    9
#define SSIZE 1024

//******* PROC status ********
#define FREE     0
#define READY    1
#define RUNNING  2
#define STOPPED  3
#define SLEEP    4
#define ZOMBIE   5

typedef struct proc{
struct proc *next;
int    *ksp;

int    uss, usp;           // at offsets 4, 6 
int    pid;                // add pid for identify the proc
int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
int    ppid;               // parent pid
struct proc *parent;
int    priority;
int    event;
int    exitCode;

char   name[32];
int    kstack[SSIZE];      // per proc stack area
}PROC;
 *******************************/

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procSize = sizeof(PROC);
int nproc = 0;

int body();
char *pname[]={"P0", "P1", "P2", "P3",  "P4", "P5","P6", "P7", "P8" };

/**************************************************
  bio.o, queue.o loader.o are in mtxlib
 **************************************************/
/* #include "bio.c" */
/* #include "queue.c" */
/* #include "loader.c" */

/**********
#include "wait.c"
#include "kernel.c"
#include "int.c"
 ************/

int do_tswitch();
int do_kfork();
int do_wait();
int do_exit()
{
   return grave();
}
int do_ps()
{
    int i = 0;
    printf("\nName\tStatus\tPID\tPPID");
    for (i = 0; i<NPROC;i++)
    {
        printf("\n%s\t",proc[i].name);
        switch(proc[i].status)
        {
            case FREE:
                printf("FREE\t");
                break;
            case READY:
                printf("READY\t");
                break;
            case RUNNING:
                printf("RUNNING\t");
                break;
            case STOPPED:
                printf("STOPPED\t");
                break;
            case SLEEP:
                printf("SLEEP\t");
                break;
            case ZOMBIE:
                printf("ZOMBIE\t");
                break;
        }
        printf("%d\t%d\n",proc[i].pid,proc[i].ppid);

    }
}
int kmode()
{
    return body();
}
int chname(int name)
{
    int i=0;
    for (i = 0;i<32;i++)
    {
        running->name[i]=get_byte(running->uss,name+i);
        if (get_byte(running->uss,name+i)==0)
            break;
        printf("%c",get_byte(running->uss,name+i));
    }
}

int init()
{
    PROC *p; int i;
    printf("init ....");
    for (i=0; i<NPROC; i++){   // initialize all procs
        p = &proc[i];
        p->pid = i;
        p->status = FREE;
        p->priority = 0;  
        strcpy(proc[i].name, pname[i]);

        p->next = &proc[i+1];
    }
    freeList = &proc[0];      // all procs are in freeList
    proc[NPROC-1].next = 0;
    readyQueue = sleepList = 0;

    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = READY;
    p->ppid   = 0;
    p->parent = p;
    running = p;
    nproc = 1;
    printf("done\n");
} 

int int80h();

int set_vec(u16 vector, u16 handler)
{
    // put_word(word, segment, offset) in mtxlib
    put_word(handler, 0, vector<<2);
    put_word(0x1000,  0, (vector<<2) + 2);
}

main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    set_vec(80, int80h);

    kfork("/bin/u1");     // P0 kfork() P1

    while(1){
        printf("P0 running\n");
        while(!readyQueue);
        printf("P0 switch process\n");
        tswitch();         // P0 switch to run P1
    }
}

int scheduler()
{
    if (running->status == READY)
        enqueue(&readyQueue, running);
    running = dequeue(&readyQueue);
}

int body()
{
    char c;
    printf("proc %d resumes to body()\n", running->pid);
    while(1){
        printf("-----------------------------------------\n");
        printList("freelist  ", freeList);
        printList("readyQueue", readyQueue);
        printList("sleepList ", sleepList);
        printf("-----------------------------------------\n");

        printf("proc %d running: parent = %d  enter a char [s|f|w|q|u] : ", 
                running->pid, running->parent->pid);

        c = getc(); printf("%c\n", c);
        switch(c){
            case 's' : do_tswitch();   break;
            case 'f' : do_kfork();     break;
            case 'w' : do_wait();      break;
            case 'q' : do_exit();      break;

            case 'u' : goUmode();      break;
        }
    }
}

int kfork(char *filename)
{
    PROC *p;
    int  i, child,j;
    u16  segment;

    /*** get a PROC for child process: ***/
    if ( (p = get_proc(&freeList)) == 0){
        printf("no more proc\n");
        return(-1);
    }

    /* initialize the new proc and its stack */
    p->status = READY;
    p->ppid = running->pid;
    p->parent = running;
    p->priority  = 1;                 // all of the same priority 1

    /******* write C code to to do THIS YOURSELF ********************
      Initialize p's kstack AS IF it had called tswitch() 
      from the entry address of body():

      HI   -1  -2    -3  -4   -5   -6   -7   -8    -9                LOW
      -------------------------------------------------------------
      |body| ax | bx | cx | dx | bp | si | di |flag|
      ------------------------------------------------------------
      ^
      PROC.ksp ---|

     ******************************************************************/
    for (j=1; j<10;j++)
        p->kstack[SSIZE-j];
    p->kstack[SSIZE-1]=(int)body;
    p->ksp = &(p->kstack[SSIZE-9]);
    enqueue(&readyQueue, p);

    // make Umode image by loading /bin/u1 into segment
    segment = (p->pid + 1)*0x1000;
    load(filename, segment);

    /*************** WRITE C CODE TO DO THESE ******************
      Initialize new proc's ustak AS IF it had done INT 80
      from virtual address 0:

      HI  -1  -2  -3  -4  -5  -6  -7  -8  -9 -10 -11 -12
      flag uCS uPC uax ubx ucx udx ubp usi udi ues uds
      0x0200 seg  0   0   0   0   0   0   0   0  seg seg
      ^
      PROC.uss = segment;           PROC.usp ----------|

     ***********************************************************/
    p->uss = segment;
    p->usp = 0x1000 - 24;
    put_word(0x0200,segment,0x1000-2);
    put_word(segment,segment,0x1000-4);
    for (j=3;j<11;j++)
        put_word(0,segment,0x1000-2*j);
    put_word(segment,segment,0x1000-22);
    put_word(segment,segment,0x1000-24);
    printf("Proc%d forked a child %d segment=%x\n", running->pid,p->pid,segment);
    return(p->pid);
}
//kforks the running proc with /bin/u1
int do_kfork()
{
    return kfork("/bin/u1");

}
//tswitches the running process
int do_tswitch()
{ 
    return tswitch();
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
            *status = (&proc[childPid])->exitCode;
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
    enqueue(&sleepList,running);
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
            enqueue(&readyQueue,&proc[i]);
            dequeue(&sleepList);
        }
    }
}

int do_wait()
{
    int i;
    wait(i);
    return i;

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
            if ((&proc[i])->status != FREE)
                return;
        }


    }
    if (running->pid != 1)
    {
        running->exitCode = 0;
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



/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
  ----------------------------------------------------------------------------
  |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
  ----------------------------------------------------------------------------
 ***************************************************************************/
#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

/****************** syscall handler in C ***************************/
int kcinth()
{
    u16    segment, offset;
    int    a,b,c,d, r;
    segment = running->uss; 
    offset = running->usp;

    /** get syscall parameters from ustack **/
    a = get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*PB);
    c = get_word(segment, offset + 2*PC);
    d = get_word(segment, offset + 2*PD);

    // UNCOMMENT TO SEE syscalls into kernel
    // printf("proc%d syscall a=%d b=%d c=%d d=%d\n", running->pid, a,b,c,d);

    switch(a){
        case 0 : r = running->pid;     break;
        case 1 : r = do_ps();          break;
        case 2 : r = chname(b);        break;
        case 3 : r = kmode();          break;
        case 4 : r = tswitch();        break;
        case 5 : r = do_wait(b);       break;
        case 6 : r = do_exit(b);       break;

        case 90: r =  getc();          break;
        case 91: r =  putc(b);         break;       
        case 99: do_exit(b);           break;
        default: printf("invalid syscall # : %d\n", a); 
    }
    put_word(r, segment, offset + 2*AX);
}
