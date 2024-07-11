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
        // printf("num_event_list: %d, i : %d\n", num_event_list, i);
        printf("Freeing job : %d\n", event_list[i].pid);
        for (int j = 0; j < event_list[i].num_cores; j++)
        {
            event_list[i].cores[j].busy = 0;
        }
        free_cores += event_list[i].num_cores;
        int lag = 0;
        // Revisar esto porque lo del lag y tal no me convence
        for (int h = 0; h < num_active_jobs; h++)
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
        num_event_list--;
        num_active_jobs--;
        // printf("num_event_list: %d\n", num_event_list);
    }
}

void insert_job()
{

    denb = 0;
    int i = 0;
    int core = 0;
    active_job[num_active_jobs] = job_queue[0];
    job = &active_job[num_active_jobs];
    printf("Job %d started\n", job->pid);
    
    // printf("Job %d has %d cores\n", job->pid, job_queue[0].events[0].num_cores);
    // printf("%d\n", job_queue[0].events[0].num_cores);
    while (core < job_queue[0].events[0].num_cores)
    {
        if (cores[i].busy == 0)
        {
            // printf("%d\n", num_active_jobs);
             //printf("%d\n", job->num_cores);
            // printf("%d\n", active_job[num_active_jobs].cores[0].id);
            active_job[num_active_jobs].cores[job->num_cores] = cores[i];

            job->num_cores++;
            cores[i].busy = 1;
            core++;
        }
        i++;
    }
     
    free_cores -= job_queue[0].events[0].num_cores;

    for (int i = 0; i < active_job[num_active_jobs].num_events; i++)
    {
        active_job[num_active_jobs].events[i] = active_job[num_active_jobs].events[i + 1];
    }
    active_job[num_active_jobs].num_events--;

    num_active_jobs++;

    for (i = 0; i < num_jobs -1; i++)
    {
        job_queue[i] = job_queue[i + 1];
    }
    num_jobs--;

    i = 0;
   
}
int scheduler()
{
    while (1)
    {
        // Meter esto en un metodo aparte porque queda guarrisimo
        if (num_jobs != 0 && job_queue[0].arrival_time <= denb && job_queue[0].events[0].num_cores <= free_cores)
        {
            insert_job();
        }
        denb++;

        // Chekear si hay algun evento nuevo en este ciclo
        if (num_event_list > 0)
        {
            free_job();
        }

        printf("Free cores: %d\n", free_cores);
        // En algun momento el num active jobs se va por debajo de 0 ns donde no encuentro pero ahi esta
        printf("Num active jobs: %d\n", num_active_jobs);
        printf("Num jobs: %d\n", num_jobs);
        if (num_jobs == 0 && num_active_jobs <= 0)
        {
            printf("All jobs completed\n");
            break;
        }
        core();
        // sleep(1);
    }

    return 0;
}