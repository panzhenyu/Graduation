#include <sched.h>
#include <unistd.h>

int set_fifo()
{
    pid_t pid = getpid();
    struct sched_param param;
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if(sched_setscheduler(pid, SCHED_FIFO, &param) < 0)
        return 0;
    return 1;
}
