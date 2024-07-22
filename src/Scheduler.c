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
            printf("Event adfasdfasdfasfasdd: %d\n", event_list[0].job.events[0].num_cores);
            if(event_list[0].job.events[event_list[0].eventnum].num_cores == 0)
            {
                printf("añjflkasjfñlksdjñklf\n");
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

void insert_job()
{

    denb = 0;
    int i = 0;
    int core = 0;

    // Meter el job en active_job
    active_job[num_active_jobs] = job_queue[0];
    job = &active_job[num_active_jobs];
    printf("Job %d started\n", job->pid);

    // Actualizar lista job_queue
    for (i = 0; i < num_jobs - 1; i++)
    {
        job_queue[i] = job_queue[i + 1];
    }
    num_jobs--;

                if(num_event_list > 0)
    {
         printf("Job id %d, event nodo %d bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n", event_list[0].job.pid, event_list[0].job.events[0].num_cores);
    }
    // Asigna cores
    while (core < active_job[num_active_jobs].events[0].num_cores)
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

    if(num_event_list > 0)
    {
         printf("Job id %d, event nodo %d AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n", event_list[0].job.pid, event_list[0].job.events[0].num_cores);
    }
    // Actualizar lista de eventos del job
    for (int i = 0; i < active_job[num_active_jobs].num_events; i++)
    {
        active_job[num_active_jobs].events[i] = active_job[num_active_jobs].events[i + 1];
    }
    active_job[num_active_jobs].num_events--;
 
    // Actualizar lista de eventos
    int added_time = event_list[0].eventtime;
    int j = 0;

    for(int i = 0; i <= active_job[num_active_jobs].num_events; i++)
    {

        if(num_event_list == 0)
        {
            event_list[0].job = active_job[num_active_jobs];
            event_list[0].eventnum = i;
            event_list[0].eventtime = active_job[num_active_jobs].events[i].time_event;
        }
        else
        {
            // no muy seguro de si esta bien asi con el if pero bueno
            // TODO reestructurar esto porque no tiene ni puto sentido, yo ya nose como va esto no tiene sentido (YO no tengo ni idea deberia dormir)
           
            while(active_job[num_active_jobs].events[i].time_event - added_time >= 0 && j < num_event_list)
            {
                j++;
                if(j != num_event_list - 1)
                {
                    added_time += event_list[j].eventtime;  
                }          
                printf("Event time: %d\n", active_job[num_active_jobs].events[i].time_event);
                printf("Added time: %d\n", added_time);
                printf("J: %d\n", j);
                printf("time_event - added_time: %d\n", active_job[num_active_jobs].events[i].time_event - added_time);
                
            }
            if(j < num_event_list)
            {
                // no muy seguro de si es > 
                for(int k = num_event_list; k > j; k--)
                {
                    printf("Job id %d, event nodo %d \n", event_list[k-1].job.pid, event_list[k-1].job.events[0].num_cores);
                    event_list[k] = event_list[k-1];
                    printf("Job id %d, event nodo %d \n", event_list[k].job.pid, event_list[k].job.events[0].num_cores);
                }

                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time + event_list[j+1].eventtime;
                event_list[j+1].eventtime -= event_list[j].eventtime;
            }else{
                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;
            }
            event_list[j].job = active_job[num_active_jobs];
            event_list[j].eventnum = i;
            

            if(event_list[j].eventtime < 0)
            {
                printf("ERROR NUMERO NEGATIVO EN TIEMPO DE EVENTO: Job id %d; Job time: %d\n", event_list[j].job.pid, event_list[j].eventtime);
                exit(1);
            }
        }
        num_event_list++;
    }

    num_active_jobs++;

    printf("Num active jobs: %d\n", num_active_jobs);
    printf("Num event list: %d\n", num_event_list);
    if(num_event_list != num_active_jobs)
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
        for(int i = 0; i < num_active_jobs; i++)
        {
            printf("    Job %d, time %d\n", active_job[i].pid, active_job[i].events[0].time_event);
        }
        printf("Num event list: %d\n", num_event_list);
        printf("    Event list:\n");
        for(int i = 0; i < num_event_list; i++)
        {
            printf("    Job %d, event %d, time %d, event jobkop %d\n", event_list[i].job.pid, event_list[i].eventnum, event_list[i].eventtime, event_list[i].job.events[event_list[i].eventnum].num_cores);
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