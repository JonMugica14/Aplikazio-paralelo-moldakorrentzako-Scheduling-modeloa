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
struct job *job_queue=NULL;
struct job *lagun;
struct job *active_job;
int free_cores;
int max_cores;
struct scheduler_events *scheduler_events;
int num_scheduler_events;
int Time;
struct job *event_list;
int num_event_list = 0;
int num_active_jobs;
int denb;

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
            printf("Data: %s\n", data);
            job_queue = (struct job *)realloc(job_queue, (num_jobs + 1) * sizeof(struct job));

            if (job_queue == NULL) {
                printf("Errore bat gertatu da realloc erabiliz\n");
                exit(1); // O manejar el error de manera más adecuada
            }
          
            tok = strtok(data, " ");
            job_queue[i].pid = atoi(tok);

            tok = strtok(NULL, " ");
            job_queue[i].arrival_time = atoi(tok);

            tok = strtok(NULL, " ");
            job_queue[i].num_events = atoi(tok);

            tok = strtok(NULL, " ");
            job_queue[i].events = (struct event *)malloc(job_queue[i].num_events * sizeof(struct event));

            j = 0;
            while (tok != NULL && j <= job_queue[i].num_events)
            {

                job_queue[i].events[j].time_event = atoi(tok);
                tok = strtok(NULL, " ");
                job_queue[i].events[j].num_cores = atoi(tok);
                tok = strtok(NULL, " ");
                j++;
            }

            data = fgets(ch, MAX_LENGTH, ptr);
            i++;
            num_jobs = i;
        }
        
    }

    //Esto es suponiendo que el número inicial de cores es el máximo al principio
    for (int i = 0; i < num_jobs; i++)
    {
        job_queue[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
    }
    
    fclose(ptr);
}

int main(int argc, char *argv[])
{
    //printf("Ezarri Sistemaren Core Zenbakia:\n");
    // scanf("%d", &num_cores);

    max_cores = 4;
    cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
    free_cores = max_cores;
    
    read_jobs();
    initialize();

    active_job = (struct job *)malloc(num_jobs * sizeof(struct job));
    event_list = (struct job *)malloc(num_jobs * sizeof(struct job));

    printf("Num Jobs: %d\n", num_jobs); 

    for (int i = 0; i < num_jobs; i++)
    {
        active_job[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
        for (int j = 0; j < max_cores; j++)
        {
            active_job[i].cores[j].id = -1;
        }
        printf("ID:%d\n", active_job[i].cores[0].id);
        
    }
    
    for (int i = 0; i < num_jobs; i++)
    {
        for (int j = 0; j < job_queue[i].num_events; j++)
        {
            printf("Job: %d, Arrival Time: %d, Event Num: %d, Event Time: %d, Num Cores: %d\n", job_queue[i].pid, job_queue[i].arrival_time, job_queue[i].num_events, job_queue[i].events[j].time_event, job_queue[i].events[j].num_cores);
        }
    }

    scheduler();
    return 0;
}