#include "timers.h"
#include <string.h>
#include "defs/defs.h"
#include "utils/critical.h"



typedef struct _Timers_t {
    timers_setup_timer_callback_t setup_timer_callback;
    timer_descr_t* timers_buffer;
    size_t timers_buffer_size;
    list_t timers_empty;
    list_t timers_queued;
    timer_descr_t* current_timer;
    timer_id_t max_timer_id;
} timers_t;

static timers_t timers;


static size_t timers_timer_id_to_index(timer_id_t tid)
{
    return (tid - 1) % timers.timers_buffer_size;
}

static timer_id_t timers_index_to_next_timer_id(size_t index)
{
    timer_id_t max_tid = timers.max_timer_id;
    
    if(max_tid > 0) max_tid --;
    
    size_t count = max_tid / timers.timers_buffer_size;
    
    timer_id_t res_tid = index + 1 + count * timers.timers_buffer_size;
    
    if(res_tid <= timers.max_timer_id) res_tid += timers.timers_buffer_size;
    
    // Переполнение.
    if(res_tid < timers.max_timer_id){
        res_tid = index + 1;
    }
    
    return res_tid;
}


static void timers_init_descrs(void)
{
    list_init(&timers.timers_empty);
    list_init(&timers.timers_queued);
    
    list_item_t* item;
    size_t i;
    
    for(i = 0; i < timers.timers_buffer_size; i ++){
        item = &timers.timers_buffer[i].list_item;
        list_item_init_data(item, (void*)i);
        list_append(&timers.timers_empty, item);
    }
}

ALWAYS_INLINE static timer_descr_t* timers_timer_descr_by_index(size_t index)
{
    if(index >= timers.timers_buffer_size) return NULL;
    return &timers.timers_buffer[index];
}

static int timers_add_compare(const void* left, const void* right)
{
    timer_descr_t* ldescr = timers_timer_descr_by_index((size_t)left);
    timer_descr_t* rdescr = timers_timer_descr_by_index((size_t)right);
    
    if(ldescr == NULL || rdescr == NULL) return 0;
    
    if(timercmp(&ldescr->t_value, &rdescr->t_value, <)) return -1;
    //if(timercmp(&ldescr->t_value, &rdescr->t_value, >)) return 1;
    return 1;
}

static void timers_remove_timer_impl(timer_descr_t* timer_descr)
{
    timer_descr->tid = INVALID_TIMER_ID;
    list_remove(&timers.timers_queued, &timer_descr->list_item);
    list_append(&timers.timers_empty, &timer_descr->list_item);
}

static void timers_move_timer_impl(timer_descr_t* timer_descr)
{
    list_remove(&timers.timers_queued, &timer_descr->list_item);
    list_insert_sorted(&timers.timers_queued, &timer_descr->list_item, timers_add_compare);
}

static timer_descr_t* timers_first_timer(void)
{
    list_item_t* timer_item = list_head(&timers.timers_queued);
    if(timer_item == NULL) return NULL;
    return timers_timer_descr_by_index((size_t)list_item_data(timer_item));
}

static timer_descr_t* timers_next_timer(timer_descr_t* timer_descr)
{
    list_item_t* timer_item = list_next(&timer_descr->list_item);
    if(timer_item == NULL) return NULL;
    return timers_timer_descr_by_index((size_t)list_item_data(timer_item));
}

static void timers_run_timer(timer_descr_t* timer_descr)
{
    if(!timer_descr->future){
        timer_descr->proc(timer_descr->arg);
    }else{
        future_start(timer_descr->future);
        void* result = timer_descr->proc(timer_descr->arg);
        future_finish(timer_descr->future, result);
    }
}

static void timers_setup_first_timer(void)
{
    timer_descr_t* cur_timer = timers_first_timer();
    
    if(!cur_timer) return;
    
    struct timeval tv_val;
    gettimeofday(&tv_val, NULL);
    
    if(timercmp(&cur_timer->t_value, &tv_val, >)){
        timersub(&cur_timer->t_value, &tv_val, &tv_val);
    }else{
        tv_val.tv_sec = 0;
        tv_val.tv_usec = 0;
    }
    
    if(timers.setup_timer_callback) timers.setup_timer_callback(&tv_val);
}

err_t timers_init(timers_init_t* timers_is)
{
    if(timers_is->buffer == NULL) return E_NULL_POINTER;
    if(timers_is->count == 0) return E_INVALID_VALUE;
    if(timers_is->setup_timer_callback == NULL) return E_NULL_POINTER;
    
    memset(&timers, 0x0, sizeof(timers_t));
    
    timers.setup_timer_callback = timers_is->setup_timer_callback;
    timers.timers_buffer = timers_is->buffer;
    timers.timers_buffer_size = timers_is->count;
    timers.max_timer_id = 0;
    
    timers_init_descrs();
    
    return E_NO_ERROR;
}

timer_id_t timers_current_timer_id(void)
{
    if(timers.current_timer == NULL) return INVALID_TIMER_ID;
    return timers.current_timer->tid;
}

timer_id_t timers_add_timer(timer_proc_t proc, struct timeval* t_start, struct timeval* t_int, void* arg, future_t* future)
{
    if(proc == NULL) return INVALID_TIMER_ID;
    if(t_start == NULL) return INVALID_TIMER_ID;
    if(!timerisset(t_start)) return INVALID_TIMER_ID;
    
    list_item_t* timer_item = NULL;
    size_t timer_index = 0;
    
    CRITICAL_ENTER();
    
    timer_item = list_head(&timers.timers_empty);
    
    if(timer_item == NULL){
        CRITICAL_EXIT();
        return INVALID_TIMER_ID;
    }
    
    list_remove(&timers.timers_empty, timer_item);
    
    CRITICAL_EXIT();
    
    timer_index = (size_t)list_item_data(timer_item);
    
    timer_descr_t* descr = timers_timer_descr_by_index(timer_index);
    
    if(descr == NULL){
        CRITICAL_ENTER();
        
        list_append(&timers.timers_empty, timer_item);
        
        CRITICAL_EXIT();
        
        return INVALID_TIMER_ID;
    }
    
    descr->proc = proc;
    descr->arg = arg;
    descr->future = future;
    
    if(t_int){
        descr->t_interval.tv_sec = t_int->tv_sec;
        descr->t_interval.tv_usec = t_int->tv_usec;
    }else{
        descr->t_interval.tv_sec = 0;
        descr->t_interval.tv_usec = 0;
    }
    
    descr->t_value.tv_sec = t_start->tv_sec;
    descr->t_value.tv_usec = t_start->tv_usec;
    
    if(descr->future){
        future_init(descr->future);
    }
    
    CRITICAL_ENTER();
    
    descr->tid = timers_index_to_next_timer_id(timer_index);
    timers.max_timer_id = descr->tid;
    list_insert_sorted(&timers.timers_queued, timer_item, timers_add_compare);
    
    CRITICAL_EXIT();
    
    if(timers.current_timer == NULL) timers_setup_first_timer();
    
    return descr->tid;
}

bool timers_remove_timer(timer_id_t tid)
{
    if(tid == INVALID_TIMER_ID) return false;
    
    size_t timer_index = timers_timer_id_to_index(tid);
    
    timer_descr_t* timer_descr = timers_timer_descr_by_index(timer_index);
    
    if(timer_descr == NULL){
        return false;
    }
    
    if(timer_descr->tid != tid) return false;
    
    CRITICAL_ENTER();
    
    if(timer_descr == timers.current_timer){
        timer_descr->t_interval.tv_sec = 0;
        timer_descr->t_interval.tv_usec = 0;
    }else{
        timers_remove_timer_impl(timer_descr);
    }
    
    CRITICAL_EXIT();
    
    if(timers.current_timer == NULL) timers_setup_first_timer();
    
    return true;
}

void timers_timer_handler(void)
{
    if(list_empty(&timers.timers_queued)) return;
    
    struct timeval tv_cur;
    gettimeofday(&tv_cur, NULL);
    
    timer_descr_t* cur_timer = timers_first_timer();
    
    for(;;){
        if(!cur_timer) break;
        
        if(timercmp(&cur_timer->t_value, &tv_cur, >)) break;
        
        timers.current_timer = cur_timer;
        
        timer_descr_t* next_timer = timers_next_timer(cur_timer);
        
        timers_run_timer(cur_timer);
        
        if(timerisset(&cur_timer->t_interval)){
            timeradd(&cur_timer->t_interval, &tv_cur, &cur_timer->t_value);
            
            CRITICAL_ENTER();
            
            timers_move_timer_impl(cur_timer);
            
            CRITICAL_EXIT();
            
        }else{
            CRITICAL_ENTER();
        
            timers_remove_timer_impl(cur_timer);

            CRITICAL_EXIT();
        }
        
        cur_timer = next_timer;
    }
    
    timers.current_timer = NULL;
    
    timers_setup_first_timer();
}
