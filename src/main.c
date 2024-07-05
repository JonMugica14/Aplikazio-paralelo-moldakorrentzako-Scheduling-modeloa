#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"

int num_cores;
int num_jobs;
struct cores *cores;
struct job *job_queue;
int free_cores;
struct scheduler_events *scheduler_events;
int num_scheduler_events;
int Time;

void initialize();

int main(int argc, char *argv[])
{

    printf("Enter the num_cores:\n");
    scanf("%d", &num_cores);
    cores = (struct cores *)malloc(num_cores * sizeof(struct cores));
    free_cores = num_cores;
    printf("Enter the num_jobs:\n");
    scanf("%d", &num_jobs);
    job_queue = (struct job *)malloc(num_jobs * sizeof(struct job));

    initialize();

    scheduler();

    return 0;
}

void initialize()
{

    for (int i = 0; i < num_cores; i++)
    {
        cores[i].id = i;
        cores[i].busy = 0;
        cores[i].time_core = rand() % 20;
    }

    for (int i = 0; i < num_jobs; i++)
    {
        job_queue[i].pid = i;
        job_queue[i].arrival_time = rand() % 20;
        job_queue[i].num_events = 1;
        job_queue[i].events = (struct event *)malloc(job_queue[i].num_events * sizeof(struct event));
        for (int j = 0; j < job_queue[i].num_events; j++)
        {
            job_queue[i].events[j].time_event = rand() % 20;
            job_queue[i].events[j].num_cores = rand() % num_cores;
        }
    }
}