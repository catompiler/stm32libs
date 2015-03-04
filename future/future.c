#include "future.h"
#include <stddef.h>
#include "utils/utils.h"

void future_init(future_t* future)
{
    future->result = NULL;
    future->done = false;
    future->running = false;
}

void* future_result(const future_t* future)
{
    return future->result;
}

void future_set_result(future_t* future, void* result)
{
    future->result = result;
}

bool future_done(const future_t* future)
{
    return future->done;
}

void future_set_done(future_t* future, bool done)
{
    future->done = done;
}

bool future_running(const future_t* future)
{
    return future->running;
}

void future_set_running(future_t* future, bool running)
{
    future->running = running;
}

void future_start(future_t* future)
{
    future->done = false;
    future->running = true;
}

void future_finish(future_t* future, void* result)
{
    future->done = true;
    future->running = false;
    future->result = result;
}

void future_wait(const future_t* future)
{
    WAIT_WHILE_TRUE(future_running(future));
}
