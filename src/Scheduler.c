#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"



int scheduler()
{
    while (1)
    {
        
        if (num_jobs != 0 && job_queue[0].arrival_time == Time && job_queue[0].events[0].num_cores <= free_cores)
        {
            Time=0;
            int i = 0;
            int core = 0;
            while (core < job_queue[0].events[0].num_cores)
            {
                if (cores[i].busy == 0)
                {
                    cores[i].job = job_queue[0];
                    cores[i].busy = 1;
                    cores[i].time_core = job_queue[0].events[0].time_event;
                    core++;
                }
                i++;
            }
            free_cores -= job_queue[0].events[0].num_cores;
            for (i = 0; i < num_jobs - 1; i++)
            {
                job_queue[i] = job_queue[i + 1];
            }
            num_jobs--;
            num_scheduler_events++;
            i = 0;
            while (i >= 0 && job_queue[0].events[0].time_event <= scheduler_events[i].time_scheduler_event)
            {
                scheduler_events[i + 1] = scheduler_events[i];
                i--;
            }
            scheduler_events[i + 1].jobs[num_jobs] = job_queue[0];
            scheduler_events[i + 1].time_scheduler_event = job_queue[0].events[0].time_event;
        }
        Time++;

        for (int i = 0; i < num_cores; i++)
        {

            if (cores[i].busy == 1)
            {
                if (cores[i].time_core == 0)
                {

                    free_cores += cores[i].job.events[0].num_cores;
                    cores[i].busy = 0;
                }
                else
                {
                    cores[i].time_core--;
                }
            }
        }
       // printf("Number of jobs left: %d\n", num_jobs);
        if(num_jobs==0)
        {
            printf("All jobs completed\n");
            break;
        }
    }
}