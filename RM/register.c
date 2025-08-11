#include <linux/sched.h>
#include <linux/syscalls.h>
#include <asm/current.h>


SYSCALL_DEFINE4(sys_register_rm, pid_t, pid, u64, period, u64, deadline, u64, exec_time)
{
    struct task_struct *p = current;
}
