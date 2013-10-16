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
  PROC *p;  int i, pid;  u16 segment;

  pid = kfork(0);   // kfork() but do NOT load any Umode image for child 
  if (pid < 0){     // kfork failed 
    return -1;
  }
  p = &proc[pid];   // we can do this because of static pid
  segment = (pid+1)*0x1000;
  copyImage(running->uss, segment, 0x1000);

  // YOUR CODE to make the child runnable in User mode  

   /**** ADD these : copy file descriptors ****/

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

