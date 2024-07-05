#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"


struct job *job;

int scheduler()
{
    while (1)
    {
        
        if (num_jobs != 0 && job_queue[0].arrival_time == denb && job_queue[0].events[0].num_cores <= free_cores)
        {
            denb=0;
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
            num_active_jobs++;

            free_cores -= job_queue[0].events[0].num_cores;

            for (i = 0; i < num_jobs - 1; i++)
            {
                job_queue[i] = job_queue[i + 1];
            }

            num_jobs--;
    
            i = 0;


            //Chekear si hay algun evento nuevo en este ciclo
            if(num_event_list > 0)
            {
                for(i = 0; i < num_event_list; i++)
                {
                    
                }              
            }

            denb++;
        }


        if(num_jobs==0)
        {
            printf("All jobs completed\n");
            break;
        }
    }
}