#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"
#include "../include/Cores.h"

struct job *job;
float bat_event=0;
float nodo_erabilpena=0;
int gertaera_ziklo=0;
int gertaera_kopurua=0;
FILE *fptr;
    
void print_info();
void insert_job(struct job *insjob);
void kalk_erabilpena();
void emaitza_lortu();

// Termina un trabajo
void free_job(struct job job, int eventnum)
{
    // Liberar los cores del trabajo
    printf("Freeing job : %d\n", job.pid);

    for (int j = 0; j < job.num_cores; j++)
    {
        job.cores[j].busy = 0;
        cores[job.cores[j].id].busy = 0;
    }
    free_cores += job.num_cores;
    int lag = 0;

    // Revisa la lista entera, cuando encuetre el trabajo a eliminar actualiza la lista moviendo todo a la izquierda borrando el trabajo
    for (int h = 0; h < num_active_jobs - 1; h++)
    {
        if (active_job[h].pid == job.pid)
        {
            lag++;
            active_job[h] = active_job[h + 1];
        }
        if (lag)
        {
            active_job[h] = active_job[h + 1];
            for (int i = 0; i < num_event_list; i++)
            {
                if (event_list[i].job->pid == active_job[h].pid)
                {
                    event_list[i].job = &active_job[h];
                }
            }
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

        free_cores++;
    }
    for (int i = num_jobs; i > 0; i--)
    {
        job_queue[i] = job_queue[i - 1];
    }
    job_queue[0] = job;
    num_jobs++;
    for (int i = num_event_list - 2; i >= 0; i--)
    {

        if (event_list[i].job->pid == job.pid)
        {
            event_list[i + 1].eventtime += event_list[i].eventtime;
            event_list[i] = event_list[i + 1];

            for (int j = i; j < num_event_list; j++)
            {
                event_list[j] = event_list[j + 1];
            }
        }
    }
    printf("Job killed\n");

    num_event_list -= job.num_events - 1;
    num_active_jobs--;
}

void block_job(struct job *job)
{
    kill_job(*job);
    for (int i = 0; i < job_queue[0].num_events; i++)
    {
        job_queue[0].events[i].time_event += event_list[1].eventtime;
    }
}

// Cambia el tamaño de un trabajo.
void update_job(struct job *job, int eventnum)
{
    printf("Updating job: %d \n", event_list[0].job->pid);

    // Checkear si el evento tiene que aumentar o disminuir cores
    if (job->events[eventnum].num_cores > job->num_cores)
    {
        printf("Adding cores\n");
        int dif = job->events[eventnum].num_cores - job->num_cores;

        if (free_cores < dif)
        {
            printf("Not enough cores to add\n");
            int lag = num_active_jobs - 1;
            int sum = free_cores;
            while (active_job[lag].pid != job->pid && lag > 0)
            {
                sum += active_job[lag].num_cores;
                lag--;
            }
            if (sum < dif)
            {
                printf("ERROR: While taking cores for %d, not enough cores to free\n", job->pid);
                block_job(job);
            }
            else
            {

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
                free_cores--;
            }
            i++;
        }
    }
    else
    {
        printf("Removing cores\n");
        // Liberar cores

        int dif = job->num_cores - job->events[eventnum].num_cores;

        for (int i = 0; i < dif; i++)
        {
            job->cores[job->num_cores - 1].busy = 0;
            cores[job->cores[job->num_cores - 1].id].busy = 0;
            job->num_cores--;
            free_cores++;
        }
    }
    printf("Job %d updated with %d cores\n", job->pid, active_job[0].num_cores);
}

// Se encarga de ver si algun evento de la lista de eventos ha terminado (el primero)
void checkevent()
{
    if (num_event_list > 0)
    {
        // Se utiliza un while para comprobar varios elementos (como si fuese un if)
        while (event_list[0].eventtime <= 0 && num_event_list > 0)
        {

            struct scheduled_events a = event_list[0];

            for (int i = 0; i < num_event_list - 1; i++)
            {
                event_list[i] = event_list[i + 1];
            }
            num_event_list--;

            // Liberar el trabajo si el evento tiene 0 cores (se checkea asi porque la lista de eventos de los trabajo no se acutalizan solo el eventnum de la lista de eventos)
            if (a.job->events[a.eventnum].num_cores <= 0)
            {
                free_job(*a.job, a.eventnum);
            }
            else
            {
                update_job(a.job, a.eventnum);
            }
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

    active_job[num_active_jobs] = *insjob;

    printf("Job %d started\n", active_job[num_active_jobs].pid);

    // Actualizar lista job_queue

    // Asigna cores

    i = 0;
    core = 0;
    active_job[num_active_jobs].num_cores = 0;

    while (core < active_job[num_active_jobs].events[0].num_cores)
    {
        if (cores[i].busy == 0)
        {

            active_job[num_active_jobs].cores[active_job[num_active_jobs].num_cores] = cores[i];

            active_job[num_active_jobs].num_cores++;

            cores[i].busy = 1;
            core++;
        }
        i++;
    }

    free_cores -= active_job[num_active_jobs].num_cores;

    // Actualizar lista de eventos del job

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Actualizar lista de eventos
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int added_time = event_list[0].eventtime;
    int j = 0; // J representa en que casilla estamos probando para meter el evento

    // Iterar por cada evento
    for (int i = 1; i <= active_job[num_active_jobs].num_events - 1; i++)
    {

        // Para el caso de que no haya eventos
        if (num_event_list == 0)
        {

            event_list[0].job = &active_job[num_active_jobs];
            event_list[0].eventnum = i;
            event_list[0].eventtime = active_job[num_active_jobs].events[i].time_event;
            added_time = event_list[0].eventtime;
        }
        else
        {

            // j = 0;
            //  Comprobamos si la casilla actual es más grande que el tiempo del evento para saber si va antes o despues. Ademas miramos si es la ultima casilla o no
            while (active_job[num_active_jobs].events[i].time_event - added_time >= 0 && j + 1 < num_event_list)
            {

                j++;
                added_time += event_list[j].eventtime;
            }

            if (j + 1 < num_event_list)
            {

                for (int k = num_event_list; k > j; k--)
                {
                    event_list[k] = event_list[k - 1];
                }

                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time + event_list[j + 1].eventtime;
                added_time += event_list[j].eventtime;
                event_list[j + 1].eventtime -= event_list[j].eventtime;
                event_list[j].job = &active_job[num_active_jobs];
                event_list[j].eventnum = i;
            }
            else
            {

                if (active_job[num_active_jobs].events[i].time_event - added_time < 0)
                {

                    event_list[j + 1] = event_list[j];
                    event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time + event_list[j + 1].eventtime;

                    added_time += event_list[j].eventtime;
                    event_list[j].job = &active_job[num_active_jobs];
                    event_list[j].eventnum = i;
                }
                else
                {

                    event_list[j + 1].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;

                    added_time += event_list[j + 1].eventtime;

                    event_list[j + 1].job = &active_job[num_active_jobs];

                    event_list[j + 1].eventnum = i;
                }
            }

            j++;

            if (event_list[j].eventtime < 0)
            {
                printf("ERROR NUMERO NEGATIVO EN TIEMPO DE EVENTO: Job id %d; Evento: %d; Job time: %d\n", event_list[j].job->pid, i, event_list[j].eventtime);
                exit(1);
            }
        }
        num_event_list++;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    num_active_jobs++;

    i = 0;
    for (i = 0; i < num_jobs - 1; i++)
    {
        job_queue[i] = job_queue[i + 1];
    }
    num_jobs--;
}

void print_info()
{
    printf("------------------------------------------------------------\n");
    printf("CICLO: %d\n", ciclototal);
    printf("Cores libres: %d\n", free_cores);

    printf("Num active jobs: %d\n", num_active_jobs);
    printf("    Active job time:\n");
    for (int i = 0; i < num_active_jobs; i++)
    {
        // esto no se actualiza con el evento actual siempre mostrara el tiempo hasta el primer evento
        printf("    Job %d, cores %d\n", active_job[i].pid, active_job[i].num_cores);
    }
    printf("Num event list: %d\n", num_event_list);
    printf("    Event list:\n");
    for (int i = 0; i < num_event_list; i++)
    {

        printf("    Job %d, event %d, time %d, event numcores %d\n", event_list[i].job->pid, event_list[i].eventnum, event_list[i].eventtime, event_list[i].job->events[event_list[i].eventnum].num_cores);
    }

    printf("Num jobs: %d\n", num_jobs);
}
int bukatzeko = 0;
int scheduler()
{
    if(ema==1)
    fptr = fopen("emaitza.txt", "w");
    while (1)
    {

        if (num_jobs != 0 && job_queue[0].arrival_time <= ciclototal && job_queue[0].events[0].num_cores <= free_cores)
        {
            insert_job(&job_queue[0]);
        }

        denb++;

        checkevent();
        if(ema==1)
        kalk_erabilpena();

        if (num_jobs == 0 && num_active_jobs <= 0)
        {
            printf("Num active jobs: %d\n", num_active_jobs);
            printf("All jobs completed\n");
            bukatzeko = 1;
            if(ema==1)
            emaitza_lortu();
            break;
        }

        print_info();

        ciclototal++;
        //usleep(10000);
    }
    if(ema==1)
    fclose(fptr);
    return 0;
}

void kalk_erabilpena()
{
    
    if (nodo_erabilpena!=(max_cores-free_cores))
    {
        bat_event+=nodo_erabilpena*gertaera_ziklo;
       
        emaitza_lortu();
        nodo_erabilpena=max_cores-free_cores;
        
        gertaera_ziklo=0;
        gertaera_kopurua++;
        
    }else
    {
        gertaera_ziklo++;
    }


}

void emaitza_lortu()
{

    

    if (fptr == NULL)
    {
        printf("Errorea fitxategia sortzean\n");
        exit(1);
    }
    if(bukatzeko==1){

        fprintf(fptr, "Emaitza: %f", bat_event/(ciclototal*max_cores));
    }else{
    fprintf(fptr, "%f\n\n", nodo_erabilpena/max_cores);
    }


 
}
