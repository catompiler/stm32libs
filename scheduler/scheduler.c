#include "scheduler.h"
#include <string.h>
#include "defs/defs.h"
#include "utils/critical.h"


typedef struct _Scheduler_t {
    task_descr_t* tasks_buffer;
    size_t tasks_buffer_size;
    list_t tasks_empty;
    list_t tasks_queued;
    task_descr_t* current_task;
    task_id_t max_task_id;
} scheduler_t;

static scheduler_t scheduler;


static size_t scheduler_task_id_to_index(task_id_t tid)
{
    return (tid - 1) % scheduler.tasks_buffer_size;
}

static task_id_t scheduler_index_to_next_task_id(size_t index)
{
    task_id_t max_tid = scheduler.max_task_id;
    
    if(max_tid > 0) max_tid --;
    
    size_t count = max_tid / scheduler.tasks_buffer_size;
    
    task_id_t res_tid = index + 1 + count * scheduler.tasks_buffer_size;
    
    if(res_tid <= scheduler.max_task_id) res_tid += scheduler.tasks_buffer_size;
    
    // Переполнение.
    if(res_tid < scheduler.max_task_id){
        res_tid = index + 1;
    }
    
    return res_tid;
}


static void scheduler_init_descrs(void)
{
    list_init(&scheduler.tasks_empty);
    list_init(&scheduler.tasks_queued);
    
    list_item_t* item;
    size_t i;
    
    for(i = 0; i < scheduler.tasks_buffer_size; i ++){
        item = &scheduler.tasks_buffer[i].list_item;
        list_item_init_data(item, (void*)i);
        list_append(&scheduler.tasks_empty, item);
    }
}

ALWAYS_INLINE static task_descr_t* scheduler_task_descr_by_index(size_t index)
{
    if(index >= scheduler.tasks_buffer_size) return NULL;
    return &scheduler.tasks_buffer[index];
}

static int tasks_add_compare(const void* left, const void* right)
{
    task_descr_t* ldescr = scheduler_task_descr_by_index((size_t)left);
    task_descr_t* rdescr = scheduler_task_descr_by_index((size_t)right);
    
    if(ldescr == NULL || rdescr == NULL) return 0;
    
    if(ldescr->priority > rdescr->priority) return -1;
    //if(ldescr->priority < rdescr->priority) return 1;
    //return 0;
    return 1;
}

err_t scheduler_init(task_descr_t* buffer, size_t count)
{
    if(buffer == NULL) return E_NULL_POINTER;
    if(count == 0) return E_INVALID_VALUE;
    
    memset(&scheduler, 0x0, sizeof(scheduler_t));
    
    scheduler.tasks_buffer = buffer;
    scheduler.tasks_buffer_size = count;
    scheduler.max_task_id = 0;
    
    scheduler_init_descrs();
    
    return E_NO_ERROR;
}

task_id_t scheduler_current_task_id(void)
{
    if(scheduler.current_task == NULL) return INVALID_TASK_ID;
    return scheduler.current_task->tid;
}

task_id_t scheduler_add_task(task_proc_t proc, task_priority_t priority, void* arg, task_flags_t flags, future_t* future)
{
    if(proc == NULL) return INVALID_TASK_ID;
    
    list_item_t* task_item = NULL;
    size_t task_index = 0;
    
    task_item = list_head(&scheduler.tasks_empty);
    
    if(task_item == NULL){
        return INVALID_TASK_ID;
    }
    
    CRITICAL_ENTER();
    
    list_remove(&scheduler.tasks_empty, task_item);
    
    CRITICAL_EXIT();
    
    task_index = (size_t)list_item_data(task_item);
    
    task_descr_t* descr = scheduler_task_descr_by_index(task_index);
    
    if(descr == NULL){
        CRITICAL_ENTER();
        
        list_append(&scheduler.tasks_empty, task_item);
        
        CRITICAL_EXIT();
        
        return INVALID_TASK_ID;
    }
    
    descr->proc = proc;
    descr->priority = priority;
    descr->arg = arg;
    descr->future = future;
    descr->flags = flags;
    
    if(descr->future){
        future_init(descr->future);
    }
    
    CRITICAL_ENTER();
    
    descr->tid = scheduler_index_to_next_task_id(task_index);
    scheduler.max_task_id = descr->tid;
    list_insert_sorted(&scheduler.tasks_queued, task_item, tasks_add_compare);
    
    CRITICAL_EXIT();
    
    return descr->tid;
}

static void scheduler_remove_task_impl(task_descr_t* task_descr)
{
    list_remove(&scheduler.tasks_queued, &task_descr->list_item);
    list_append(&scheduler.tasks_empty, &task_descr->list_item);
}

bool scheduler_remove_task(task_id_t tid)
{
    if(tid == INVALID_TASK_ID) return false;
    
    size_t task_index = scheduler_task_id_to_index(tid);
    
    task_descr_t* task_descr = scheduler_task_descr_by_index(task_index);
    
    if(task_descr == NULL){
        return false;
    }
    
    if(task_descr->tid != tid) return false;
    
    if(task_descr == scheduler.current_task){
        task_descr->flags |= TASK_RUN_ONCE;
    }else{
        CRITICAL_ENTER();
        
        scheduler_remove_task_impl(task_descr);
        
        CRITICAL_EXIT();
    }
    
    return true;
}

static task_descr_t* scheduler_first_task(void)
{
    list_item_t* task_item = list_head(&scheduler.tasks_queued);
    if(task_item == NULL) return NULL;
    return scheduler_task_descr_by_index((size_t)list_item_data(task_item));
}

static task_descr_t* scheduler_next_task(task_descr_t* task_descr)
{
    list_item_t* task_item = list_next(&task_descr->list_item);
    if(task_item == NULL) return NULL;
    return scheduler_task_descr_by_index((size_t)list_item_data(task_item));
}

static task_descr_t* scheduler_select_task_by_priority(task_descr_t* task_descr_l, task_descr_t* task_descr_r)
{
    if(task_descr_l->priority > task_descr_r->priority) return task_descr_l;
    return task_descr_r;
}

bool scheduler_process(void)
{
    if(list_empty(&scheduler.tasks_queued)) return false;
    
    task_descr_t* cur_task = NULL;
    
    if(scheduler.current_task){
        cur_task = scheduler.current_task;
    }else{
        cur_task = scheduler_first_task();
        if(cur_task == NULL) return false;
        scheduler.current_task = cur_task;
    }
    
    if(!cur_task->future){
        cur_task->proc(cur_task->arg);
    }else{
        future_start(cur_task->future);
        void* result = cur_task->proc(cur_task->arg);
        future_finish(cur_task->future, result);
    }
    
    task_descr_t* next_task = scheduler_next_task(cur_task);
    
    if(cur_task->flags & TASK_RUN_ONCE){
        CRITICAL_ENTER();
        
        scheduler_remove_task_impl(cur_task);
        
        CRITICAL_EXIT();
    }
    
    task_descr_t* first_task = scheduler_first_task();
    
    if(next_task == NULL){
        cur_task = first_task;
    }else{
        if(first_task != NULL){
            cur_task = scheduler_select_task_by_priority(first_task, next_task);
        }// Ошибочное состояние.
        else{
            CRITICAL_ENTER();
            
            // Сбросим задачи.
            scheduler_init_descrs();
            
            CRITICAL_EXIT();
            
            cur_task = NULL;
        }
    }
    
    scheduler.current_task = cur_task;
    
    return true;
}
