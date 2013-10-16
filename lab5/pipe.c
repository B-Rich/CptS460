show_pipe(PIPE *p)
{
    int i, j;
    printf("Data: %d \n", p->data);
    printf("Room: %d \n", p->room);
    printf("------------ PIPE CONTENETS ------------\n");     
    printf(p->buf);
    printf("\n----------------------------------------\n");
}

char *MODE[ ]={"READ_PIPE ","WRITE_PIPE"};

int pfd()
{
    int i;
    printf("fd\ttype\tmode\n----\t----\t----\n");
    for (i = 0;i<NFD;i++)
    {
        if (oft[i].refCount != 0)
        {
            printf("%d\tPIPE\t",i);
            if (oft[i].mode == READ_PIPE)
                printf("READ\n");
            else
                printf("WRITE\n");
        }
    }
}

//reads a pipe 
int read_pipe(int fd, char *buf, int n)
{
    int i;
    char buff[1024];
    for (i = 0;i<n;i++)
    {
        while(oft[fd].pipe_ptr->data == 0)
        {
            sleep(oft[fd].pipe_ptr); 
        }
        printf("head: %d",oft[fd].pipe_ptr->head);
        buff[i] = oft[fd].pipe_ptr->buf[(i+oft[fd].pipe_ptr->head)%10]; 
        put_byte(buff[i],running->uss,buf+i);
        oft[fd].pipe_ptr->room++;
        oft[fd].pipe_ptr->data--;

    }
    buff[i]=0;
    oft[fd].pipe_ptr->head += n;

    
    printf("Read: ");
    printf(buff);
    printf(" from pipe\n");
    
    wakeup(oft[fd].pipe_ptr);    
    show_pipe(oft[fd].pipe_ptr);
    return i;

}
//writes to a pipe
int write_pipe(int fd, char *buf, int n)
{
    // your code for write_pipe()
    int i;

    for (i = 0; i<n;i++)
    {
        while (oft[fd].pipe_ptr->room == 0)
        {
            //sleep until room is available
            printf("room: %d",oft[fd].pipe_ptr->room);
            sleep(oft[fd].pipe_ptr);
            
        }

        oft[fd].pipe_ptr->buf[(i+oft[fd].pipe_ptr->tail)%10]=get_byte(running->uss,buf+i);
        oft[fd].pipe_ptr->room--;
        oft[fd].pipe_ptr->data++;
    }

    oft[fd].pipe_ptr->tail = oft[fd].pipe_ptr->tail + n;
    show_pipe(oft[fd].pipe_ptr);
    wakeup(oft[fd].pipe_ptr);
    return n;
}
//creates a pipe with two ends
int kpipe(int pd[2])
{
    // create a pipe; fill pd[0] pd[1] (in USER mode!!!) with descriptors
    int i=0,j = 0;
    pd[0]=-1;
    pd[1]=-1;
    for (i=0;i<NPIPE;i++)
    {
        if (pipe[i].busy==0)
        {
            j = i;

            pipe[i].head = 0;
            pipe[i].tail = 0;
            pipe[i].room = 10;
            pipe[i].busy = 1;

            break;

        }
    }
    for (i=0; i < NOFT ;i++)
    {
        if (oft[i].refCount == 0)
        {
            pd[0] = i;
            oft[i].refCount = 1;
            oft[i].mode = READ_PIPE;
            running->fd[i]=&oft[i];
            oft[i].pipe_ptr = &pipe[j];
            oft[i].pipe_ptr->nreader = i; 
            break; 
        }
    }
    for (i=0; i<NOFT;i++)
    {
        if (oft[i].refCount == 0)
        {
            pd[1] = i;
            oft[i].refCount = 1;
            oft[i].mode = WRITE_PIPE;
            running->fd[i]=&oft[i];
            oft[i].pipe_ptr = &pipe[j];
            oft[i].pipe_ptr->nwriter = i;
            break;
        }
    }
    return pd;

}

int close_pipe(int fd)
{
    OFT *op; PIPE *pp;

    printf("proc %d close_pipe: fd=%d\n", running->pid, fd);

    op = running->fd[fd];
    running->fd[fd] = 0;                 // clear fd[fd] entry 

    if (op->mode == READ_PIPE){
        pp = op->pipe_ptr;
        pp->nreader--;                   // dec n reader by 1

        if (--op->refCount == 0){        // last reader
            if (pp->nwriter <= 0){         // no more writers
                pp->busy = 0;             // free the pipe   
                return;
            }
        }
        wakeup(&pp->room); 
        return;
    }

    if (op->mode == WRITE_PIPE){
        pp = op->pipe_ptr;
        pp->nwriter--;                   // dec nwriter by 1

        if (--op->refCount == 0){        // last writer 
            if (pp->nreader <= 0){         // no more readers 
                pp->busy = 0;              // free pipe also 
                return;
            }
        }
        wakeup(&pp->data);
        return;
    }
}
