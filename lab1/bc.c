/*******************************************************
*                        @bc.c file                                  *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode         INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLK 1024
char buf1[BLK], buf2[BLK], name[64];

int prints(char *s)
{
     while(*s)
         putc(*s++);
}
int scans(char *s)
{
    int i = 0;
    char c = getc();
    while(c!=0 && c!='\r')
    {
        s[i]=c;
        i++;
        putc(c);
        c = getc(); 
    }
    s[i]=0;
}

u16 getblk(u16 blk, char *buf)
{
        readfd( blk/18, ((blk*2)%36)/18, ((blk*2)%36)%18, buf);
}
char temp[256];
main()
{ 
    u16    i,iblk;
    char   c;
    GD    *gp;
    INODE *ip;
    DIR   *dp;

    prints("read decsriptor block #2 into buf1[]\n\r");
    getblk(2, buf1);
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;
    prints("inodes blk = "); putc(iblk + '0'); prints("\n\r");

    getblk((u16)iblk, buf1);  // read first inode block block
    ip = (INODE *)buf1 + 1;   // ip->root inode #2
    
    prints("read 0th data block of root inode into buf2[ ]\n\r");

    getblk((u16)ip->i_block[0], buf2); 
    dp = (DIR *)buf2;           // buf2 contains DIR entries

    while((char *)dp < &buf2[BLK]){

        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        prints(temp); putc(' ');

        /*
        c = dp->name[dp->name_len];
     v   dp->name[dp->name_len] = 0;
        prints(dp->name); putc(' ');
        dp->name[dp->name_len] = c;
        */
        dp = (DIR *)((char *)dp + dp->rec_len);
    }

    prints("\n\rEnter name of boot image: ");
    scans(name);
}
