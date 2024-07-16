#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"

int core()
{
    printf("Active jobs: \n");
    for (int i = 0; i < num_active_jobs; i++)
    {
        printf("Job id: %d, time_event: %d, event cores: %d\n", active_job[i].pid, active_job[i].events[0].time_event, active_job[i].events[0].num_cores);

        if (active_job[i].events[0].time_event <= 0)
        {
            event_list[num_event_list] = active_job[i];
            num_event_list++;
            printf("Job %d finished\n", active_job[i].pid);
        }
        else
        {
            active_job[i].events[0].time_event--;
        }
    }

    return 0;
}