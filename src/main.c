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

FILE* ptr;
char  *data, ch[MAX_LENGTH], *tok;

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
    ptr = fopen("data/jobs.txt", "r");

    if(ptr == NULL)
    {
        printf("Fitxategia irakurtzen errore bat gertatu da\n");

    }else
    {
        data = fgets(ch, MAX_LENGTH, ptr);
        data = fgets(ch, MAX_LENGTH, ptr);
        int i = 0;
        int j;
        while(data != EOF){ 
            tok = strtok(data, " ");
            job_queue[i].pid = atoi(tok);
            tok = strtok(data, " ");
            job_queue[i].arrival_time = atoi(tok);
            tok = strtok(data, " ");
            job_queue[i].num_events = atoi(tok);
            tok = strtok(data, " ");
            j = 0;
            while(tok != NULL)
            {
                job_queue[i].events[j].time_event = atoi(tok);
                tok = strtok(data, " ");
                job_queue[i].events[j].num_cores = atoi(tok);
                tok = strtok(data, " ");
                j++;
            }
            i++;
        }   
        num_jobs = i;
    }
}

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
    read_jobs();

    scheduler();    
    return 0;
}