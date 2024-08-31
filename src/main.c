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
int blocked = 0;

FILE *ptr;
char *data, ch[MAX_LENGTH], *tok;

void initialize()
{

    for (int i = 0; i < max_cores; i++)
    {
        cores[i].id = i;
        cores[i].busy = 0;
    }
}
void serial(int paralell)
{
    int events_num, a, b;

    FILE *fptrs, *fptr;
    fptrs = fopen("serial.txt", "w");
    fptr = fopen("jobs.txt", "r");

    if (fptr == NULL)
    {
        printf("Errorea fitxategia sortzean\n");
        exit(1);
    }

    data = fgets(ch, MAX_LENGTH, fptr);
    fprintf(fptrs, "PID ArrivalTime NumEvents\n");
    data = fgets(ch, MAX_LENGTH, fptr);

    while (data != NULL)
    {
       
        tok = strtok(data, " ");

        a = atoi(tok);

        tok = strtok(NULL, " ");
        b = atoi(tok);
        tok = strtok(NULL, " ");
        events_num = atoi(tok);
        fprintf(fptrs, "%d %d %d\n", a, b, 2);
        data = fgets(ch, MAX_LENGTH, fptr);
        tok = strtok(data, " ");
        int i = 0;
        int sum = 0;
        int cores=max_cores;
        while (tok != NULL && i <= events_num - 1)
        {

            sum += atoi(tok);
            i++;
            tok = strtok(NULL, " ");
            if(paralell==1&&i==1){
                cores=atoi(tok);
                printf("Cores:%d\n", cores);
            }
            fgets(ch, MAX_LENGTH, fptr);
            tok = strtok(ch, " ");
        }
       
        fprintf(fptrs, "%d %d\n", 0, cores);
        fprintf(fptrs, "%d %d\n", sum, 0);
        fprintf(fptrs, "\n");
        
        data = fgets(ch, MAX_LENGTH, fptr);
        
    }

    fclose(fptrs);
    fclose(fptr);
}
void read_jobs(int file)
{
    if (file == 0)
        ptr = fopen("jobs.txt", "r");
    else if (file == 1){
        
        ptr = fopen("serial.txt", "r");}
    if (ptr == NULL)
    {
        printf("Fitxategia irakurtzen errore bat gertatu da\n");
        exit(1);
    }
    else
    {
        printf("Lectura de archivo correcta\n");
        
    

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

            // Para diferenciar entre el tiempo de entrada y el de eventos
            while (tok != NULL && j <= job_queue[i].num_events - 1)
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
    event_list = (struct scheduled_events *)malloc((num_jobs * 10) * sizeof(struct scheduled_events));

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
    int bat = 0;
    for (int i = 1; i <= n; i++)
    {
        bat = 0;
        int num_eventss = rand() % 10;
        fprintf(fptr, "%d %d %d\n", i, rand() % 20, num_eventss);
        fprintf(fptr, "0 %d\n", rand() % max_cores);
        for (int i = 1; i < num_eventss - 1; i++)
        {
            bat += rand() % 20;
            fprintf(fptr, "%d %d\n", bat, rand() % (max_cores - 1) + 1);
        }
        bat += rand() % 20;

        fprintf(fptr, "%d %d", bat, 0);
        if (i != n)
        {
            fprintf(fptr, "\n\n");
        }
    }
    fclose(fptr);
}

int main(int argc, char *argv[])
{
    int ziklo_par_mal, ziklo_par, ziklo_ser;
    max_cores = 20;
    cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
    free_cores = max_cores;
    printf("---------------------------\n");
    printf("Comienza el simulador:\n");

    printf("---------------------------\n");
    printf("Maleable\n");
    generateJob(5);
    read_jobs(0);

    printf("---------------------------\n");
    initialize();

    scheduler();

    printf("Ciclos totales: %d\n", ciclototal);
    ziklo_par_mal=ciclototal;
    for (int i = 0; i < num_jobs; i++)
    {
        free(job_queue[i].events);
    }
    free(job_queue);

    free(cores);

    free(active_job);

    free(event_list);
    printf("---------------------------\n");
    printf("No maleable\n");
    printf("---------------------------\n");
    ciclototal=0;
    serial(1);

    read_jobs(1);
    
    initialize();
    scheduler();
   
    printf("Ciclos totales: %d\n", ciclototal);
    ziklo_par=ciclototal;
    for (int i = 0; i < num_jobs; i++)
    {
        free(job_queue[i].events);
    }
    free(job_queue);

    free(cores);

    free(active_job);

    free(event_list);
     printf("---------------------------\n");
    printf("Serie\n");
    printf("---------------------------\n");
    ciclototal=0;
    serial(0);

    read_jobs(1);
    
    initialize();
    scheduler();
   
    printf("Ciclos totales: %d\n", ciclototal);
    ziklo_ser=ciclototal;
    printf("---------------------------\n");
    printf("Estos han sido los resultados de la simulación:\n");
    printf("Maleable:%d\n", ziklo_par_mal);
    printf("No maleable:%d\n", ziklo_par);
    printf("Serie:%d\n", ziklo_ser);
    printf("---------------------------\n");
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