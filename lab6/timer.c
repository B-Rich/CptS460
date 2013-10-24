/* timer parameters. */
#define LATCH_COUNT     0x00	/* cc00xxxx, c = channel, x = any */
#define SQUARE_WAVE     0x36	/* ccaammmb, a = access, m = mode, b = BCD */

/************************ NOTICE THE DIVISOR VALUE ***********************/
#define TIMER_FREQ   1193182L	/* timer frequency for timer in PC and AT */
#define TIMER_COUNT ((unsigned) (TIMER_FREQ/60)) /* initial value for counter*/

#define TIMER0       0x40
#define TIMER_MODE   0x43
#define TIMER_IRQ       0


int enable_irq(irq_nr) unsigned irq_nr;
{
  lock();
    out_byte(0x21, in_byte(0x21) & ~(1 << irq_nr));

}

/*===========================================================================*
 *				timer_init				     *
 *===========================================================================*/

ushort tick;
ushort on;
ushort fdc;
int timerCount = 0;

int timer_init()
{
  /* Initialize channel 0 of the 8253A timer to e.g. 60 Hz. */

  printf("timer init\n");
  tick = 0;

  out_byte(TIMER_MODE, SQUARE_WAVE);	/* set timer to run continuously */
  out_byte(TIMER0, TIMER_COUNT);	/* load timer low byte */
  out_byte(TIMER0, TIMER_COUNT >> 8);	/* load timer high byte */
  enable_irq(TIMER_IRQ); 
}

/*===========================================================================*
 *				timer_handler				     *
 *===========================================================================*/

int thandler()
{
    int i;
    PROC* p;
    tick++;
    
    tick %= 60;

    if (tick % 60 == 0)
    {
        fdc ++;
        fdc %= 5;
        setClock(timerCount);
        timerCount++;
        //decrement the sleep time of everything in the sleep queue by 1
        p = sleepList;
        while(p!=0)
        {
            if (p->time == 0)
            {    
                //printf("waking on %d\n", p);
                wakeup(p);
            }
            else
                p->time--;
            //printf("sleeping for %d more seconds\n",p->time);
            p = p->next;
        }
        //decrement the running proccess by one if it is in umode
        if (inkmode == 1)
        {
            if (running->time == 0)
            {
                out_byte(0x20,0x20);
                running->time = 5;
                tswitch();
            }
            else
                running->time--;
        }
        // every 5 seconds toggle fd1
        if (on && fdc == 0)
        {
            printf("floopy off\n");
            out_byte(0x0c,0x3f2);
            on = 0;
        }
        else if (!on && fdc == 0)
        {
            printf("floopy on\n");
            out_byte(0x1c,0x3f2);
            on = 1;
        }
    }
    out_byte(0x20, 0x20);  

}





