#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

struct job *job;

void free_job(struct job job, int eventnum)
{


        printf("Freeing job : %d\n", job.pid);
        for (int j = 0; j < job.num_cores; j++)
        {
            job.cores[j].busy = 0;
        }
        free_cores += job.num_cores;
        int lag = 0;
        for (int h = 0; h < num_active_jobs; h++)
        {
            if (active_job[h].pid == job.pid)
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

void update_job();

void checkevent()
{
    if(num_event_list > 0)
    {   
        while (event_list[0].eventtime <= 0 && num_event_list > 0)
        {
            if(event_list[0].job.events[event_list[0].eventnum].num_cores == 0)
            {
                free_job(event_list[0].job, event_list[0].eventnum);
            }
            else
            {
                //update_job();
            }
            for(int i = 0; i < num_event_list; i++)
            {
                event_list[i] = event_list[i+1];
            }
            num_event_list--;
        }
        if (num_event_list > 0)
        event_list[0].eventtime--;
    }
}

// TODO meter en la lista de eventos los eventos de cada job
void insert_job()
{

    denb = 0;
    int i = 0;
    int core = 0;
    active_job[num_active_jobs] = job_queue[0];
    job = &active_job[num_active_jobs];
    printf("Job %d started\n", job->pid);

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

    free_cores -= job_queue[0].events[0].num_cores;

    for (int i = 0; i < active_job[num_active_jobs].num_events; i++)
    {
        active_job[num_active_jobs].events[i] = active_job[num_active_jobs].events[i + 1];
    }
    active_job[num_active_jobs].num_events--;
 
    int added_time = event_list[0].eventtime;
    int j = 0;

    for(int i = 0; i <=active_job[num_active_jobs].num_events; i++)
    {

        if(num_event_list == 0)
        {
            event_list[0].job = active_job[num_active_jobs];
            event_list[0].eventnum = i;
            event_list[0].eventtime = active_job[num_active_jobs].events[i].time_event;
        }
        else
        {
            // Chekear si es berdin 0
            j++;
            while(active_job[num_active_jobs].events[i].time_event - added_time >= 0 && j < num_event_list)
            {

                added_time += event_list[j].eventtime;                
                printf("Event time: %d\n", active_job[num_active_jobs].events[i].time_event);
                printf("Added time: %d\n", added_time);
                j++;
            }
            for(int k = num_event_list; k > j; k--)
            {
                event_list[k] = event_list[k-1];
            }
            event_list[j].job = active_job[num_active_jobs];
            event_list[j].eventnum = i;
            event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;
        }
        num_event_list++;
    }

    num_active_jobs++;

    

    for (i = 0; i < num_jobs - 1; i++)
    {
        job_queue[i] = job_queue[i + 1];
    }
    num_jobs--;

    i = 0;
}

void print_info()
{
     printf("Free cores: %d\n", free_cores);

        printf("Num active jobs: %d\n", num_active_jobs);
        printf("    Active job time:\n");
        for(int i = 0; i < num_active_jobs; i++)
        {
            printf("    Job %d, time %d\n", active_job[i].pid, active_job[i].events[0].time_event);
        }
        printf("Num event list: %d\n", num_event_list);
        printf("    Event list:\n");
        for(int i = 0; i < num_event_list; i++)
        {
            printf("    Job %d, event %d, time %d\n", event_list[i].job.pid, event_list[i].eventnum, event_list[i].eventtime);
        }

        printf("Num jobs: %d\n", num_jobs);
}

int scheduler()
{
    while (1)
    {

        if (num_jobs != 0 && job_queue[0].arrival_time <= ciclototal && job_queue[0].events[0].num_cores <= free_cores)
        { 
            insert_job();
        }
        denb++;

        checkevent();

       
        if (num_jobs == 0 && num_active_jobs <= 0)
        {
            printf("All jobs completed\n");
            break;
        }
        // Creo que esto no hace falta. Si calculamos desde aquí cuanto falta hasta cada evento, no hace falta el core
        //core();
        print_info();
      
        ciclototal++;
    }

    return 0;
}