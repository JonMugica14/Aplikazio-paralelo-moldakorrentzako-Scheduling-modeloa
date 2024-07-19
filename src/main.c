#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

#define MAX_LENGTH 512

// Declarariones de variables

int num_cores;
int num_jobs;
struct cores *cores;
struct job *job_queue = NULL;
struct job *lagun;
struct job *active_job;
int free_cores;
int max_cores;
struct scheduler_events *scheduler_events;
int num_scheduler_events;
int Time;
struct scheduled_events *event_list;
int num_event_list = 0;
int num_active_jobs = 0;
int denb;
int ciclototal = 0;

FILE *ptr;
char *data, ch[MAX_LENGTH], *tok;

void initialize()
{

    for (int i = 0; i < num_cores; i++)
    {
        cores[i].id = i;
        cores[i].busy = 0;
    }
}

void read_jobs()
{
    ptr = fopen("jobs.txt", "r");

    if (ptr == NULL)
    {
        printf("Fitxategia irakurtzen errore bat gertatu da\n");
        exit(1);
    }
    else
    {
        data = fgets(ch, MAX_LENGTH, ptr);
        data = fgets(ch, MAX_LENGTH, ptr);

        int i = 0;
        int j;

        while (data != NULL)
        {

            job_queue = (struct job *)realloc(job_queue, (num_jobs + 1) * sizeof(struct job));

            if (job_queue == NULL)
            {
                printf("Errore bat gertatu da realloc erabiliz\n");
                exit(1);
            }

            tok = strtok(data, " ");
            job_queue[i].pid = atoi(tok);

            tok = strtok(NULL, " ");
            job_queue[i].arrival_time = atoi(tok);

            tok = strtok(NULL, " ");
            job_queue[i].num_events = atoi(tok);
            job_queue[i].events = (struct event *)malloc(job_queue[i].num_events * sizeof(struct event));

            data = fgets(ch, MAX_LENGTH, ptr);

            tok = strtok(data, " ");
            j = 0;

            while (tok != NULL && j <= job_queue[i].num_events)
            {
                job_queue[i].events[j].time_event = atoi(tok);
                tok = strtok(NULL, " ");
                job_queue[i].events[j].num_cores = atoi(tok);

                data = fgets(ch, MAX_LENGTH, ptr);

                tok = strtok(data, " ");

                j++;
            }
            data = fgets(ch, MAX_LENGTH, ptr);
            i++;
            num_jobs = i;
        }
    }

    for (int i = 0; i < num_jobs; i++)
    {
        job_queue[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
    }

    fclose(ptr);

    active_job = (struct job *)malloc(num_jobs * sizeof(struct job));
    event_list = (struct scheduled_events *)malloc(num_jobs * sizeof(struct scheduled_events));

    printf("Num Jobs: %d\n", num_jobs);

    for (int i = 0; i < num_jobs; i++)
    {
        active_job[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
        for (int j = 0; j < max_cores; j++)
        {
            active_job[i].cores[j].id = -1;
        }
    }
}

void generateJob(int n)
{

    FILE *fptr;
    fptr = fopen("jobs.txt", "w");

    if (fptr == NULL)
    {
        printf("Errorea fitxategia sortzean\n");
        exit(1);
    }

    fprintf(fptr, "PID ArrivalTime NumEvents\n");

    for (int i = 1; i <= n; i++)
    {
        fprintf(fptr, "%d %d %d\n", i, rand() % 20, 1);
        fprintf(fptr, "0 %d\n", rand() % max_cores);
        fprintf(fptr, "%d %d\n", rand() % 20, 0);
        fprintf(fptr, "\n");
    }
    fclose(fptr);
}

int main(int argc, char *argv[])
{

    int create_jobs;
    max_cores = 20;
    cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
    free_cores = max_cores;
    printf("---------------------------\n");
    printf("Sartu lan kopurua:\n");
    scanf("%d", &create_jobs);
    printf("---------------------------\n");
    generateJob(create_jobs);
    sleep(1);
    read_jobs();
    initialize();

    for (int i = 0; i < num_jobs; i++)
    {
        printf("Job: %d, Arrival Time: %d\n", job_queue[i].pid, job_queue[i].arrival_time);

        for (int j = 0; j <= job_queue[i].num_events; j++)
        {
            printf("   Event time: %d, Num Cores: %d\n", job_queue[i].events[j].time_event, job_queue[i].events[j].num_cores);
        }
    }

    scheduler();

    printf("Ziklo kopurua: %d\n", ciclototal);

    for (int i = 0; i < num_jobs; i++)
    {

        free(job_queue[i].events);
    }
    free(job_queue);

    free(cores);

    free(active_job);
    free(event_list);

    return 0;
}