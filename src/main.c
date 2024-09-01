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
int ema;
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
void serial(int paralell, int n)
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
        int j=0;
        int sum = 0;
        int cores=max_cores;
        int max=0;
        while (tok != NULL   && i <= events_num - 1 )
        {

            sum += atoi(tok);
            i++;
            tok = strtok(NULL, " ");
            
                if(max<atoi(tok))
                max=atoi(tok);
                
            
            fgets(ch, MAX_LENGTH, fptr);
            tok = strtok(ch, " ");
        }
       if(paralell==0) max=max_cores;
        fprintf(fptrs, "%d %d\n", 0, max);
        fprintf(fptrs, "%d %d", sum, 0);
         
        if (a != n)
        {
            fprintf(fptrs, "\n\n");
        }
       
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
        
    

        data = fgets(ch, MAX_LENGTH, ptr);
    
        data = fgets(ch, MAX_LENGTH, ptr);
        int i = 0;
        int j;
        num_jobs=0;
        while (data != NULL && strlen(data)!=0)
        {
            num_jobs++;
            job_queue = (struct job *)realloc(job_queue, (num_jobs) * sizeof(struct job));
            
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

            if (job_queue[i].events == NULL)
            {
                printf("Errore bat gertatu da malloc erabiliz\n");
                exit(1);
            }
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
            
        }
        
    }

    for (int i = 0; i < num_jobs; i++)
    {
        job_queue[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
         if (job_queue[i].cores == NULL)
        {
            printf("Errore bat gertatu da malloc erabiliz\n");
            exit(1);
        }
    }

    fclose(ptr);

    active_job = (struct job *)malloc(num_jobs * sizeof(struct job));
     if (active_job == NULL)
    {
        printf("Errore bat gertatu da malloc erabiliz\n");
        exit(1);
    }
    event_list = (struct scheduled_events *)malloc((num_jobs * 10) * sizeof(struct scheduled_events));
     if (event_list == NULL)
    {
        printf("Errore bat gertatu da malloc erabiliz\n");
        exit(1);
    }
  //  printf("Num Jobs: %d\n", num_jobs);

    for (int i = 0; i < num_jobs; i++)
    {
        active_job[i].cores = (struct cores *)malloc(max_cores * sizeof(struct cores));
        if (active_job[i].cores == NULL)
        {
            printf("Errore bat gertatu da malloc erabiliz\n");
            exit(1);
        }
        for (int j = 0; j < max_cores; j++)
        {
            active_job[i].cores[j].id = -1;
        }
    }
    
    //    printf("Lectura de archivo correcta\n");
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
        int num_eventss = rand() % 8 +2;
        fprintf(fptr, "%d %d %d\n", i, rand() % 20, num_eventss);
        fprintf(fptr, "0 %d\n", rand() % (max_cores-1)+1);
        for (int i = 1; i < num_eventss - 1; i++)
        {
            bat += rand() % 20;
            fprintf(fptr, "%d %d\n", bat, rand() % (max_cores - 1) + 1);
        }
        bat += ((rand() % 20)+1);

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

    
    ema=1;
    int generate=atoi(argv[1]);
    int mode=atoi(argv[2]);
    
    
   // generate=50;
    printf("%d", atoi(argv[2]));
if(atoi(argv[2])==0){
    srand(time(NULL));
    printf(";MASFMMFAKFMSKFM\n");
    generateJob(generate);
}
    if(mode==1){

    printf("---------------------------\n");
    printf("No maleable\n");
    printf("---------------------------\n");
    ema=1;
    ciclototal=0;
    serial(1, generate);

    read_jobs(1);
    
    initialize();
    scheduler();

    }else if(mode==2){

 printf("---------------------------\n");
    printf("Serie\n");
    printf("---------------------------\n");
    ema=1;
    ciclototal=0;
    serial(0, generate);

    read_jobs(1);
    
    initialize();
    scheduler();

    }else{

    printf("---------------------------\n");
    printf("Maleable\n");
    printf("---------------------------\n");
    ema=1;
 read_jobs(0);
    
    initialize();
    scheduler();

    }
    
    
   
    printf("Ciclos totales: %d\n", ciclototal);
    ziklo_par_mal=ciclototal;
    printf("\n\n\n\n\n");
    
    for (int i = 0; i < num_jobs; i++)
    {
        free(job_queue[i].events);
    }

     for (int i = 0; i < num_jobs; i++)
    { free(active_job[i].cores);
    }
    free(job_queue);

    free(cores);

    free(active_job);

    free(event_list);
    /*
    printf("---------------------------\n");
    printf("No maleable\n");
    printf("---------------------------\n");
    ema=0;
    ciclototal=0;
    serial(1, generate);

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
    serial(0, generate);

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
    */
    return 0;
}