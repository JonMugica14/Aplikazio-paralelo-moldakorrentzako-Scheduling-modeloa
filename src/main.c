#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

int num_cores;
int num_jobs;
struct cores *cores;
struct job *job_queue;
struct job *active_job;
int free_cores;
int max_cores;
struct scheduler_events *scheduler_events;
int num_scheduler_events;
int Time;
struct job *event_list;
int num_event_list;
int num_active_jobs;
int denb;



void initialize();

int main(int argc, char *argv[])
{

     

    printf("Ezarri Sistemaren Core Zenbakia:\n");
    scanf("%d", &num_cores);
    cores = (struct cores *)malloc(num_cores * sizeof(struct cores));
    free_cores = num_cores;
    printf("Ezarri Sistemaren Lan Kopurua:\n");
    scanf("%d", &num_jobs);
    job_queue = (struct job *)malloc(num_jobs * sizeof(struct job));


    num_active_jobs = 0;
    max_cores = num_cores;
    denb = 0;
    num_event_list = 0;

    
    active_job = (struct job *)malloc(num_jobs * sizeof(struct job));
    event_list = (struct job *)malloc(num_jobs * sizeof(struct job));

    initialize();
    

    scheduler();


    free(cores);
    free(job_queue);
    free(active_job);
    free(event_list);
    
    return 0;
}

void initialize()
{

    for (int i = 0; i < num_cores; i++)
    {
        cores[i].id = i;
        cores[i].busy = 0;
    }
        printf("Lista de trabajos:");
    // TODO cambiar esto para que no sea aleatorio
    for (int i = 0; i < num_jobs; i++)
    {

        job_queue[i].pid = i;
        job_queue[i].arrival_time = rand() % 20;
        job_queue[i].num_events = 1;
        job_queue[i].events = (struct event *)malloc(job_queue[i].num_events * sizeof(struct event));
        
        job_queue[i].events[1].time_event = rand() % 20;
        job_queue[i].events[1].num_cores = 0; //Simular que el job ha terminado
        printf("Pid: %d\n", job_queue[i].pid);
        printf("Arrival time: %d\n", job_queue[i].arrival_time);
        printf("Num events: %d\n", job_queue[i].num_events);
        printf("Time event: %d\n", job_queue[i].events[0].time_event);
        printf("Num cores: %d\n", job_queue[i].events[0].num_cores);

        /*for (int j = 0; j < job_queue[i].num_events; j++)
        {
            job_queue[i].events[j].time_event = rand() % 20;
            job_queue[i].events[j].num_cores = rand() % num_cores;
        }*/
    }
}