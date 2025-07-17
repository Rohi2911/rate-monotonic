#include "sched.h"

static void
enqueue_task_rm(struct rq *rq, struct task_struct *p, int flags) {
    
    struct rm_rq *rm_rq = &rq->rm;
    struct sched_rm_entity *rm_se = &p->rm;

    list_add_tail(&rm_se->run_list, &rm_rq->active.queue[rm_se->prio]);
	__set_bit(rm_se->prio, rm_rq->active.bitmap);

    rm_rq->nr_running++;


}

static void
dequeue_task_rm(struct rq *rq, struct task_struct *p, int flags) {

    struct rm_rq *rm_rq = &rq->rm;
    struct sched_rm_entity *rm_se = &p->rm;

    list_del_init(&rm_se->run_list);

    if(list_empty(&(rm_rq->active.queue[rm_se->prio]))) {
        __clear_bit(rm_se->prio, &rm_rq->active);
    }

    rm_rq->rm_nr_running--;

}

static void 
check_preempt_curr_rm(struct rq *rq, struct task_struct *p, int flags) {

    if(p->prio < rq->curr->prio) {
        resched_curr(rq);
        return;
    }
}

static void yield_task_rm(struct rq *rq) {
    
}


static inline void 
set_next_task_rm(struct rq *rq, struct task_struct *p, bool first) {

}

static struct task_struct *pick_next_task_rm(struct rq *rq) {

}

static void put_prev_task_rm(struct rq *rq, struct task_struct *p) {

}

#ifdef CONFIG_SMP

static int
select_task_rq_rm(struct task_struct *p, int cpu, int flags) {

}

#endif /* CONFIG_SMP */

static void update_curr_rm(struct rq *rq) {

}

static void task_tick_rm(struct rq *rq, struct task_struct *p, int queued) {

}


DEFINE_SCHED_CLASS(rm) = {

    .enqueue_task           = enqueue_task_rm,
    .dequeue_task           = dequeue_task_rm,
    .yield_task             = yield_task_rm,
    .check_preempt_curr     = check_preempt_curr_rm,
    .pick_next_task         = pick_next_task_rm,
    .put_prev_task          = put_prev_task_rm,
    .set_next_task          = set_next_task_rm,

#ifdef CONFIG_SMP
    .select_task_rq         = select_task_rq_rm,

#endif /* CONFIG_SMP */

    .task_tick              = task_tick_rm,
    .update_curr            = update_curr_rm,
};