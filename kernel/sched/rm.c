#include "sched.h"

#define for_each_sched_rm_entity(rm_se) \
	for (; rm_se; rm_se = NULL)

static inline 
struct task_struct *rm_task_of(struct sched_rm_entity *rm_se) {

    return container_of(rm_se, struct task_struct, rm);
}

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

static void 
requeue_rm_entity(struct rm_rq *rm_rq, struct sched_rm_entity *rm_se, int head) {

    if(rm_se->on_rq) {
        struct rm_prio_array *array = &rm_rq->active;
        struct list_head *queue = array->queue + rm_task_of(rm_se)->prio;

        if(head) {
            list_move(&rm_se->run_list, queue);
        }
        else {
            list_move_tail(&rm_se->run_list, queue);
        }
    }
}

static void 
requeue_task_rm(struct rq *rq, struct task_struct *p, int head) {

    struct sched_rm_entity *rm_se = &p->rm;
    struct rm_rq *rm_rq;

    for_each_sched_rm_entity(rm_se) {
        rm_rq = rm_se->rm_rq;
        requeue_rm_entity(rm_rq, rm_se, head);
    }
}

static void yield_task_rm(struct rq *rq) {
    
    requeue_task_rm(rq, rq->curr, 0);
}


static inline void 
set_next_task_rm(struct rq *rq, struct task_struct *p, bool first) {

    p->se.exec_start = rq_clock_task(rq);

}

static struct task_struct *pick_next_task_rm(struct rq *rq) {

    struct rm_rq *rm_rq = &rq->rm;
    struct sched_rm_entity *next = NULL;
    struct rm_prio_array *array = &rm_rq->active;
    struct list_head *queue;
    int idx;

    struct task_struct *p;

    idx = sched_find_first_bit(array->bitmap);
    
    if(idx >= MAX_RM_PRIO) {
        return NULL;
    }

    queue = array->queue + idx;
    next = list_entry(queue->next, struct sched_rm_entity, run_list);
    p = rm_task_of(next);

    set_next_task_rm(rm_rq, p, true);
    return p;

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