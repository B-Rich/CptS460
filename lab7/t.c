#include "type.h"

PROC proc[NPROC], *freeList, *running, *readyQueue, *sleepList;
int procSize = sizeof(PROC);

/******************** use your OWN LAB5 code *****************
 #include "io.c"
 #include "queue.c"
 #include "wait.c"   
 #include "fork_exec.c"
*************************************************************/
PROC* getproc();
PROC* get_proc(PROC * list);
void enqueue(PROC **queue,PROC *p);
PROC * dequeue(PROC **queue);
int goUmode();

#include "int.c"
#include "pv.c"
#include "serial.c"
#include "kbd.c"

int body();

char *tname[]={"Mercury", "Venus", "Earth",  "Mars", "Jupiter", 
               "Saturn",  "Uranus","Neptune"};
int init()
{
   int i,j; 
   PROC *p;
   for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->pid = i;
       p->status = FREE;
       p->next = &proc[i+1];
       strcpy(p->name, tname[i]);
       printf("%d %s ", p->pid, proc[i % 4].name);
   }
   printf("\n");

   proc[NPROC-1].next = 0;
   freeList = &proc[0];   
   readyQueue = 0;

   // create and run P0
   p = getproc();
   p->pri = 0;
   p->status = READY;
   p->inkmode = 1;
   running = p;
}

// enters a proc into the queue specified according to it's priority
void enqueue(PROC ** queue,PROC *p)
{
    PROC* past; 
    PROC* temp; 
    past = *queue; 
    if ( past == NULL || p->pri > past->pri )
    { 
        p->next = past; 
        *queue = p; return; 
    } 
    temp = past->next; 
    while(temp != NULL && temp->pri >= p->pri ) 
    { 
        past = past->next; 
        temp = temp->next; 
    } 
    past->next = p;
    p->next = temp;
 /* This function is very broken so I am replacing it with code that I borrowed
  * from David Barajas
    int i=0;
    PROC * q = *queue;
    PROC * tmp;
    PROC * tmp2;
    printf("queue: %d",*queue);
    if (*queue == 0) 
    {
        *queue = p;
        return;
    }
    //add the new proc pointer to the end of the array
    while(q!=0)
    {
        if  (p->priority > q->priority)
        {
            //shuffle everything down one and stick p in the current place
            tmp = q;
            q=p;
            q = q->next;
            for (i;i<NPROC;i++)
            {
                tmp2 = q;
                q = tmp;
                tmp = tmp2;
                if (tmp = 0)
                {
                    break;//break out if we are at the empty section of queue
                }
                q = q->next;
            }
        }
        else
        {
            i++;
        }

    }
    q = p;
*/
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
            enqueue(&freeList,&proc[childPid]);
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
//calls wait
int kwait()
{
    int i;
    wait(i);
    return i;

}

//causes the process to exit
int kexit()
{
   return grave();
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

//execs a given file path
int kexec(int path)
{
        
    PROC *p;
    int  i, child,j;
    u16  segment;
    char file_path[32];
    
    p = running;
    /*** get a PROC for child process: ***/

    /* initialize the new proc and its stack */

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
    p->kstack[SSIZE-1]=(int)goUmode;
    p->ksp = &(p->kstack[SSIZE-9]);
    enqueue(&readyQueue, p);

    // make Umode image by loading the file designated
    segment = (p->pid + 1)*0x2000;    
    for (i = 0;i<32;i++)
    {
        file_path[i]=get_byte(running->uss,path+i);
        if (get_byte(running->uss,path+i)==0)
            break;
    }
    printf("\nloading %s\n",file_path);
    load(file_path,segment);



    //gotta fix that segment to make sure it doesnt reference wrong stuff
    p->uss = segment;
    p->usp = 0x2000 - 24;
    put_word(0x0200,segment,0x2000-2);
    put_word(segment,segment,0x2000-4);
    for (j=3;j<11;j++)
        put_word(0,segment,0x2000-2*j);
    put_word(segment,segment,0x2000-22);
    put_word(segment,segment,0x2000-24);


    return p->pid;


}


//dequeues the given queue
PROC* dequeue(PROC ** queue)
{
    PROC * p = *queue;
    *queue = (*queue)->next;
    printf("dequeued, previous proc: %d, next: %d\n",p->pid,(*queue)->pid);
    return p;
}

PROC* get_proc(PROC * list)
{
    return dequeue(list);
}

PROC* getproc()
{
    return get_proc(freeList);
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
  
int body()
{
   char c, CR, buf[64];
   while(1){
      printf("=======================================\n");
      printQueue(readyQueue);      

      printf("proc %d %s in Kmode\n", running->pid, running->name);
      printf("input a command (s|f|u|q|i|o) : ");
      c=getc(); putc(c); CR=getc(); putc(CR);
      switch(c){
          case 's' : tswitch(); break;
          case 'u' : printf("\nProc %d ready to go U mode\n", running->pid);
                     goUmode(); break;
          case 'f':  myfork();  break;
          case 'q' : kexit();   break;
          case 'i' : iline();   break;
          case 'o' : oline();   break;
      }
   }
}

//kfork!
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
    printf("new proc at %d\n",p);

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
    printf("enqueuing proc\n");
    enqueue(&readyQueue, p);
    printf("done enqueueing \n");

    // make Umode image by loading /bin/u1 into segment
    segment = (p->pid + 1)*0x2000;
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
    p->usp = 0x2000 - 24;
    put_word(0x0200,segment,0x2000-2);
    put_word(segment,segment,0x2000-4);
    for (j=3;j<11;j++)
        put_word(0,segment,0x2000-2*j);
    put_word(segment,segment,0x2000-22);
    put_word(segment,segment,0x2000-24);
    printf("Proc%d forked a child %d segment=%x\n", running->pid,p->pid,segment);
    return(p->pid);
}







/***************************************************************
  myfork(segment) creates a child task and returns the child pid.
  When scheduled to run, the child task resumes to body(pid) in 
  K mode. Its U mode environment is set to segment.
****************************************************************/
int myfork()
{  
   int child;
   child = kfork("/bin/u1");
   if (child < 0){
       printf("myfork failed\n"); 
       return;
   } 
   printf("task %d return from myfork() : child=%d\n", running->pid, child);
}



int int80h(), s0inth(),kbinth();  // int s1inth() // for serial port 1

int set_vec(vector, addr) ushort vector, addr;
{
    ushort location,cs;
    location = vector << 2;
    cs = getcs();
    put_word(addr, 0, location);
    put_word(cs,0,location+2);
}

main()
{
    int pid;
    resetVideo();
    printf("\nWelcome to the 460 Multitasking System\n");
    printf("initializing ......\n");
        init();
    printf("initialization complete\n");

    set_vec(80, int80h);
    printf("Interupt vector installed\n");
    pid = myfork(); 

    set_vec(12, s0inth);
    //set_vec(11, s1inth); // for second serial port at 0x2F8
    sinit();
    set_vec(9, kbinth);
    kbinit();
    while(1){
       if (readyQueue)
       {
            printf("switching\n");
            tswitch();
         }
    }
    printf("all dead, happy ending\n");
}

int scheduler()
{ 
    if (running->status == READY)
       enqueue(&readyQueue,running);
    running = dequeue(&readyQueue);
}


int oline()
{
  // assume port = 0;
  sputline(0, "serial line from Kmode\n\r");
}

char kline[64];
int iline()
{
  sgetline(0, kline);
  printf("\nkline=%s\n", kline);
}
