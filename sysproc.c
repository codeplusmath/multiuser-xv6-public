#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_lseek(void)
{
	struct file * fp;
	int fd, x, ex, res;
	signed int offset, newoff;
	char * pt;

	newoff = 0;

	argfd(0, &fd, &fp);
	argint(1, &offset);
	argint(2, &x);

	if(x == SEEK_SET){
		newoff = offset;
		fp->off = newoff;
	}

	else if(x == SEEK_CUR){
		newoff = fp->off + offset;
		fp->off = newoff;
	}

	else if(x == SEEK_END){
		newoff = fp->ip->size + offset;
	}

	if(fp->ip->size < newoff){
		ex = newoff - fp->ip->size;
		pt = kalloc();
		while(ex > 0){
			filewrite(fp, pt, ex);
			ex = ex - 4096;
		}
		kfree(pt);
	}

	fp->off = newoff;
	res = fp->off;
	return res;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
