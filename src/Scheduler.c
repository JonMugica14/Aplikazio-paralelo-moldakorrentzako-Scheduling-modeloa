#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"


int time = 0;


int scheduler()
{
    while(1)
    {
        if(num_jobs != 0 && job_queue[0].arrival_time == time && job_queue[0].events[0].num_cores <= free_cores)
        {
            int i = 0;
            int core = 0;
            while(core < job_queue[0].events[0].num_cores)
            {
                if(cores[i].busy == 0)
                {
                    cores[i].job = job_queue[0];
                    cores[i].busy = 1;
                    cores[i].time = job_queue[0].events[0].time;
                    core++;
                }
                i++;
            }
            free_cores -= job_queue[0].events[0].num_cores;
            for(i = 0; i < num_jobs - 1; i++)
            {
                job_queue[i] = job_queue[i + 1];
            }
            num_jobs--;
            num_scheduler_events++;
            i = 0;
            while(i>=0 && job_queue[0].events[0].time <= scheduler_events[i].time)
            {
                scheduler_events[i + 1] = scheduler_events[i];
                i--;
            }
            scheduler_events[i + 1].jobs[num_jobs] = job_queue;
            scheduler_events[i + 1].time = job_queue[0].events[0].time;

        }

        if()
    }
}