//int goUmode(void);

int copyImage(u16 s1,u16 s2, u16 size)
{
    int i = 0;
    u16 word;
    while (i<=size)
    {
        word = get_word(s1,i);
        put_word(word,s2,i);
        i+=2;
    }

}

int fork()
{
    PROC *p;
    int i, j, pid;
    u16 segment;

    pid = kfork(0);   // kfork() but do NOT load any Umode image for child 
    if (pid < 0){     // kfork failed 
        return -1;
    }
    p = &proc[pid];   // we can do this because of static pid

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
    p->kstack[SSIZE-1]=(int)goUmode;
    
    segment = (pid+1)*0x1000;
    copyImage(running->uss, segment, 32*1024);

    // YOUR CODE to make the child runnable in User mode  

    /**** ADD these : copy file descriptors ****/
    p->uss = segment;
    p->usp = 0x1000 - 24;
    put_word(0x0200,segment,0x1000-2);
    put_word(segment,segment,0x1000-4);
    for (j=3;j<11;j++)
        put_word(0,segment,0x1000-2*j);
    put_word(segment,segment,0x1000-22);
    put_word(segment,segment,0x1000-24);

    for (i=0; i<NFD; i++){
        p->fd[i] = running->fd[i];
        if (p->fd[i] != 0){
            p->fd[i]->refCount++;
            if (p->fd[i]->mode == READ_PIPE)
                p->fd[i]->pipe_ptr->nreader++;
            if (p->fd[i]->mode == WRITE_PIPE)
                p->fd[i]->pipe_ptr->nwriter++;
        }
    }
    return(p->pid);
}

extern int loader();

int exec(char *filename)
{
    // your exec function
}

