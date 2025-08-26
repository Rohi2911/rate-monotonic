#include <linux/sched.h>
#include <linux/syscalls.h>
#include <asm/current.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rcupdate.h>

struct rm_task {
    struct sched_rm_entity *rm_se;
    struct list_head list;
};

static LIST_HEAD(rm_task_list);
static DEFINE_MUTEX(rm_task_lock);

static long rm_total_util =   0;
static int rm_task_count    =   0;

long power(long base, long exponent) {
    // Computes base^exponent using iterative multiplication
    long result = 1;
    int i;
    int steps = 10000; // Increase for better precision
    long frac_exp = exponent;

    // Use exponential approximation for fractional exponents
    // (We use the formula: base^exp = exp(exp * ln(base)), but avoid math.h)
    // So, we use exp(x) ≈ 1 + x + x^2/2 + x^3/6 + ... up to some terms
    long x = frac_exp * 1; //(0.69314718056); // ln(2) ≈ 0.69314718056
    long term = 1;
    long sum = 1;
    for (i = 1; i < 20; i++) {
        term *= x / i;
        sum += term;
    }
    return (long)sum;
}

static long comp_schedulability_bound(int n)
{

    return n * (power(2, 1 / n) - 1);
}

SYSCALL_DEFINE4(register_rm, pid_t, pid, u64, period, u64, deadline, u64, exec_time)
{
    struct task_struct      *p;
    struct rm_task          *new_task;
    struct sched_rm_entity  *rm_se;
    long util;
    long new_total;

    rcu_read_lock();
    p = find_task_by_vpid(pid);
    if(p == NULL) {
        rcu_read_unlock();
        return -22;
    }
    
    get_task_struct(p);
    rcu_read_unlock();
    
    util = exec_time / period;

    mutex_lock(&rm_task_lock);
    new_total = rm_total_util + util;

    if(new_total > comp_schedulability_bound(rm_task_count + 1)) {
        mutex_unlock(&rm_task_lock);
        put_task_struct(p);
        return -22;
    }

    rm_se = &p->rm;
    rm_se->rm_runtime   = exec_time;
    rm_se->rm_deadline  = deadline;
    rm_se->rm_period    = period;

    new_task = kzalloc(sizeof(*new_task), GFP_KERNEL);
    INIT_LIST_HEAD(&new_task->list);
    list_add_tail(&new_task->list, &rm_task_list);

    rm_total_util = new_total;
    rm_task_count++;

    mutex_unlock(&rm_task_lock);
    return 0;
}
