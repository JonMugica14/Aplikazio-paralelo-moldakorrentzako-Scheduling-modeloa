#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

struct job *job;
void print_info();
void insert_job(struct job *insjob);

void free_job(struct job job, int eventnum)
{
    printf("Freeing job : %d\n", job.pid);
    for (int j = 0; j < job.num_cores; j++)
    {
        job.cores[j].busy = 0;
        cores[job.cores[j].id].busy = 0;
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

// Mata un trabajo activo pero no terminado para hacer hueco para uno más importante
void kill_job(struct job job)
{
    printf("Killing job: %d\n", job.pid);
    for (int i = 0; i < job.num_cores; i++)
    {
        job.cores[i].busy = 0;
        cores[job.cores[i].id].busy = 0;
        job.num_cores--;
        free_cores++;
    }
    for (int i = num_jobs; i > 0; i--)
    {
        job_queue[i] = job_queue[i - 1];
    }
    job_queue[0] = job;
    num_jobs++;
    for (int i = 0; i < num_event_list; i++)
    {
        if (event_list[i].job.pid == job.pid)
        {
            event_list[i + 1].eventtime += event_list[i].eventtime;
            event_list[i] = event_list[i + 1];

            for (int j = i; j < num_event_list; j++)
            {
                event_list[j] = event_list[j + 1];
            }
            num_event_list--;
        }
    }
    num_active_jobs--;
}

void block_job(struct job *job)
{
    kill_job(*job);
    for(int i = 0; i < job_queue[0].num_events; i++)
    {
        job_queue[0].events[i].time_event += event_list[0].eventtime;
    }
    insert_job(&job_queue[0]);
}

void update_job()
{
    job = &event_list[0].job;
    if (job->events[event_list[0].eventnum].num_cores > job->num_cores)
    {
        int dif = job->events[event_list[0].eventnum].num_cores - job->num_cores;
        if (free_cores < dif)
        {
            int lag = num_active_jobs - 1;
            int sum = free_cores;
            while (active_job[lag].pid != job->pid)
            {
                sum += active_job[lag].num_cores;
                lag++;
            }
            if (sum < dif)
            {
                printf("ERROR: While taking cores for %d, not enough cores to free\n", job->pid);
                block_job(job);
                blocked = 1;
            }else
            {
                blocked = 0;
                while (dif > free_cores && num_active_jobs > 0)
                {
                    kill_job(active_job[num_active_jobs - 1]);
                }
            }

        }
        int i = 0;
        while (dif > 0)
        {
            if (cores[i].busy == 0)
            {
                job->cores[job->num_cores] = cores[i];
                job->num_cores++;
                cores[i].busy = 1;
                dif--;
            }
            i++;
        }
    }
    else
    {
        // Liberar cores
        int dif = job->num_cores - job->events[event_list[0].eventnum].num_cores;
        for (int i = 0; i < dif; i++)
        {
            job->cores[job->num_cores - 1].busy = 0;
            cores[job->cores[job->num_cores - 1].id].busy = 0;
            job->num_cores--;
            free_cores++;
        }
    }
}

void checkevent()
{
    if (num_event_list > 0)
    {
        while (event_list[0].eventtime <= 0 && num_event_list > 0)
        {

            if (event_list[0].job.events[event_list[0].eventnum].num_cores == 0)
            {

                free_job(event_list[0].job, event_list[0].eventnum);
            }
            else
            {
                update_job();
            }
            for (int i = 0; i < num_event_list; i++)
            {
                event_list[i] = event_list[i + 1];
            }
            num_event_list--;
        }
        if (num_event_list > 0)
            event_list[0].eventtime--;
    }
}

void insert_job(struct job *insjob)
{

    denb = 0;
    int i = 0;
    int core = 0;

    // Meter el job en active_job
    active_job[num_active_jobs] = *insjob;
    job = &active_job[num_active_jobs];
    printf("Job %d started\n", active_job[num_active_jobs].pid);

    // Actualizar lista job_queue
    for (i = 0; i < num_jobs - 1; i++)
    {
        job_queue[i] = job_queue[i + 1];
    }
    num_jobs--;

    // Asigna cores

    i = 0;
    while (core < active_job[num_active_jobs].events[0].num_cores)
    {
        if (cores[i].busy == 0)
        {

            active_job[num_active_jobs].cores[job->num_cores] = cores[i];

            active_job[num_active_jobs].num_cores++;
            printf("Num cores: %d\n", active_job[num_active_jobs].num_cores);
            cores[i].busy = 1;
            core++;
        }
        i++;
    }
    
    free_cores -= active_job[num_active_jobs].num_cores;

    // Actualizar lista de eventos del job
    for (int i = 0; i < active_job[num_active_jobs].num_events; i++)
    {
        active_job[num_active_jobs].events[i] = active_job[num_active_jobs].events[i + 1];
    }
    active_job[num_active_jobs].num_events--;

    // Actualizar lista de eventos
    int added_time = event_list[0].eventtime;
    int j = 0;

    for (int i = 0; i <= active_job[num_active_jobs].num_events; i++)
    {

        if (num_event_list == 0)
        {
            event_list[0].job = active_job[num_active_jobs];
            event_list[0].eventnum = i;
            event_list[0].eventtime = active_job[num_active_jobs].events[i].time_event;
        }
        else
        {
            j = 0;
            while (active_job[num_active_jobs].events[i].time_event - added_time >= 0 && j < num_event_list)
            {
                j++;
                if (j != num_event_list - 1)
                {
                    added_time += event_list[j].eventtime;
                }
                printf("Event time: %d\n", active_job[num_active_jobs].events[i].time_event);
                printf("Added time: %d\n", added_time);
                printf("J: %d\n", j);
                printf("time_event - added_time: %d\n", active_job[num_active_jobs].events[i].time_event - added_time);
            }
            if (j < num_event_list)
            {

                for (int k = num_event_list; k > j; k--)
                {
                    event_list[k] = event_list[k - 1];
                }

                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time + event_list[j + 1].eventtime;
                event_list[j + 1].eventtime -= event_list[j].eventtime;
            }
            else
            {
                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;
            }
            event_list[j].job = active_job[num_active_jobs];
            event_list[j].eventnum = i;

            if (event_list[j].eventtime < 0)
            {
                printf("ERROR NUMERO NEGATIVO EN TIEMPO DE EVENTO: Job id %d; Job time: %d\n", event_list[j].job.pid, event_list[j].eventtime);
                exit(1);
            }
        }
        num_event_list++;
    }

    num_active_jobs++;

    if (num_event_list != num_active_jobs)
    {
        printf("ERROR: Event list and active jobs are not the same size\n");
        print_info();
        exit(1);
    }

    i = 0;
}

void print_info()
{
    printf("Free cores: %d\n", free_cores);

    printf("Num active jobs: %d\n", num_active_jobs);
    printf("    Active job time:\n");
    for (int i = 0; i < num_active_jobs; i++)
    {
        printf("    Job %d, time %d\n", active_job[i].pid, active_job[i].events[0].time_event);
    }
    printf("Num event list: %d\n", num_event_list);
    printf("    Event list:\n");
    for (int i = 0; i < num_event_list; i++)
    {
        printf("    Job %d, event %d, time %d, event jobkop %d\n", event_list[i].job.pid, event_list[i].eventnum, event_list[i].eventtime, event_list[i].job.events[event_list[i].eventnum].num_cores);
    }

    printf("Num jobs: %d\n", num_jobs);
}

int scheduler()
{
    while (1)
    {

        if (num_jobs != 0 && job_queue[0].arrival_time <= ciclototal && job_queue[0].events[0].num_cores <= free_cores && blocked == 0)
        {
            insert_job(&job_queue[0]);
        }

        denb++;

        checkevent();

        if (num_jobs == 0 && num_active_jobs <= 0)
        {
            printf("All jobs completed\n");
            break;
        }

        print_info();

        ciclototal++;
        
    }

    return 0;
}