#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

struct job *job;

void free_job()
{
    for (int i = num_event_list - 1; i >= 0; i--)
    {
        for (int j = 0; event_list[i].num_cores; j++)
        {
            event_list[i].cores[j].busy = 0;
            free_cores++;
        }

        int lag = 0;
        for (int h = 0; i < num_active_jobs; i++)
        {
            if (active_job[h].pid == event_list[i].pid)
            {
                lag++;
                active_job[h] = active_job[h + 1];
            }
            if (lag)
            {
                active_job[h] = active_job[h + 1];
            }
        }
        num_active_jobs--;
    }
    num_event_list--;
}


int scheduler()
{
    while (1)
    {
       
        if (num_jobs != 0 && job_queue[0].arrival_time == denb && job_queue[0].events[0].num_cores <= free_cores)
        {
           
            denb = 0;
            int i = 0;
            int core = 0;
            active_job[num_active_jobs] = job_queue[0];
            job = &active_job[num_active_jobs];
    
            while (core < job_queue[0].events[0].num_cores)
            {
                if (cores[i].busy == 0)
                {
                    active_job[num_active_jobs].cores[job->num_cores] = cores[i];
                    job->num_cores++;
                    cores[i].busy = 1;
                    core++;
                }
                i++;
            }
            for (int i = 0; i < job_queue[0].num_events; i++)
            {
                job_queue[0].events[i] = job_queue[0].events[i + 1];
            }
            job_queue[0].num_events--;

            num_active_jobs++;

            free_cores -= job_queue[0].events[0].num_cores;

            for (i = 0; i < num_jobs - 1; i++)
            {
                job_queue[i] = job_queue[i + 1];
            }

            num_jobs--;

            i = 0;
        }
        
        denb++;
       
        // Chekear si hay algun evento nuevo en este ciclo
        if (num_event_list > 0)
            free_job();

        if (num_jobs == 0)
        {
            printf("All jobs completed\n");
            break;
        }    

        core();
    }
}