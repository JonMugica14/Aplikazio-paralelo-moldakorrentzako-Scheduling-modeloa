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

// Termina un trabajo
void free_job(struct job job, int eventnum)
{
    //Liberar los cores del trabajo
    printf("Freeing job : %d\n", job.pid);
    
    for (int j = 0; j < job.num_cores; j++)
    {
        job.cores[j].busy = 0;
        cores[job.cores[j].id].busy = 0;
    }
    free_cores += job.num_cores;
    int lag = 0;

    // Revisa la lista entera, cuando encuetre el trabajo a eliminar actualiza la lista moviendo todo a la izquierda borrando el trabajo
    for (int h = 0; h < num_active_jobs-1; h++)
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
      printf("EventList4uiole: %d\n", event_list[0].job->pid);  
}

// Mata un trabajo activo pero no terminado para hacer hueco para uno más importante
void kill_job(struct job job)
{
    printf("Killing job: %d\n", job.pid);
    printf("num_cores: %d\n", job.num_cores);
    for (int i = 0; i < job.num_cores; i++)
    {
        printf("Ciclo: %d\n", i);
        job.cores[i].busy = 0;
        cores[job.cores[i].id].busy = 0;
        //job.num_cores--; Quitado para que no interfiera con el bucle creo que no hace falta hacer esto porque se borra
        free_cores++;
    }
    for (int i = num_jobs; i > 0; i--)
    {
        job_queue[i] = job_queue[i - 1];
    }
    job_queue[0] = job;
    num_jobs++;
    for (int i = num_event_list - 2; i >= 0; i--) // Esto esta porque mirar si el primero es el que se va a borrar no es necesario dado que no hay que sustituirlo por nada
    {   
        printf("alsjfklasjflk\n");
        if (event_list[i].job->pid == job.pid)
        {
            event_list[i+1].eventtime += event_list[i].eventtime;
            event_list[i] = event_list[i + 1];
            
            for (int j = i; j < num_event_list; j++)
            {
                event_list[j] = event_list[j + 1];
            }
            printf("Event deleted\n");
        }
       
    }
    printf("Job killed\n");
    printf("Num events: %d\n", job.num_events);
    num_event_list-=job.num_events -1;
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
        printf("Dif: %d\n", dif);
        if (free_cores < dif)
        {
            printf("Not enough cores to add\n");
            int lag = num_active_jobs - 1;
            int sum = free_cores;
            while (active_job[lag].pid != job->pid && lag >0)
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
                    printf("free_cores: %d\n", free_cores);
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
        printf("Num cores: %d\n", job->num_cores);
        printf("event_list[0].eventnum: %d\n", eventnum);
        printf("Event num cores: %d\n", job->events[eventnum].num_cores);
        int dif = job->num_cores - job->events[eventnum].num_cores;
        printf("Dif: %d\n", dif);
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


//Se encarga de ver si algun evento de la lista de eventos ha terminado (el primero)
void checkevent()
{
    if (num_event_list > 0)
    {
        // Se utiliza un while para comprobar varios elementos (como si fuese un if)
        while (event_list[0].eventtime <= 0 && num_event_list > 0)
        {
            // Para Jon = Aquí has puesto eventime pero aquí hay que mirar si los cores de este cambio son 0 o no, para saber si el trabajo termina o no. f (event_list[0].eventtime == 0)
           struct scheduled_events a= event_list[0];
           printf("Event job pid: %d\n", event_list[1].job->pid);
           for (int i = 0; i < num_event_list-1; i++)
            {
                event_list[i] = event_list[i + 1];
            }
            num_event_list--;
            printf("Event job pid: %d\n", event_list[0].job->pid);
            // Liberar el trabajo si el evento tiene 0 cores (se checkea asi porque la lista de eventos de los trabajo no se acutalizan solo el eventnum de la lista de eventos)
            if (a.job->events[a.eventnum].num_cores <= 0)
            {
                free_job(*a.job,a.eventnum);
                printf("Event job pid121212: %d\n", a.job->pid);
            }
            else
            {
                update_job(a.job, a.eventnum);
            }
            printf("event_list[0].pid: %d\n", a.job->pid);
            
            printf("num_event_list: %d\n", num_event_list);
            printf("event_list[0].pid: %d\n", event_list[0].job->pid);
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
if(num_event_list != 0) {
    printf("PID121212:: %d\n", event_list[0].job);
    printf("Direccion de event_list[0].job: %p\n", event_list[0].job);
}
    // Meter el job en active_job
   printf("Direccion de active_job[%d]: %p\n", num_active_jobs, (void*)&active_job[num_active_jobs]);
    active_job[num_active_jobs] = *insjob;
   
   
 
    if(num_event_list!=0){
 printf("PID121212:: %d\n", event_list[0].job->pid);
 printf("PID121212:: %d\n", event_list[0].job->events[0].time_event);
    }
    printf("Job %d started\n", active_job[num_active_jobs].pid);
    printf("Num events: %d\n", num_event_list);
    printf("Num active jobs: %d\n", num_active_jobs);
    printf("Active job info: %d\n", active_job[0].pid);

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
            printf("Num cores: %d\n", active_job[num_active_jobs].num_cores);

            cores[i].busy = 1;
            core++;
        }
        i++;
    }

    free_cores -= active_job[num_active_jobs].num_cores;
    
    // Actualizar lista de eventos del job 

    // ESTO NO SE PUEDE HACER PORQUE AL BORRAR UN TRABAJO DE LA LISTA DE EVENTOS AL VOLVER A EMPEZAR SE PIERDE EL PRIMER EVENTO
    /*for (int i = 0; i < active_job[num_active_jobs].num_events; i++)
    {
        active_job[num_active_jobs].events[i] = active_job[num_active_jobs].events[i + 1];
    }
    active_job[num_active_jobs].num_events--;
    */

    // Actualizar lista de eventos
    int added_time = event_list[0].eventtime;
    int j = 0; // J representa en que casilla estamos probando para meter el evento

    // Iterar por cada evento
    for (int i = 1; i <= active_job[num_active_jobs].num_events - 1; i++)
    {
        // Para el caso de que no haya eventos
        if (num_event_list == 0)
        {
            printf("active_job:%p\n", active_job[num_active_jobs]);
            event_list[0].job = &active_job[num_active_jobs];
            event_list[0].eventnum = i;
            event_list[0].eventtime = active_job[num_active_jobs].events[i].time_event;
            added_time = event_list[0].eventtime;
            printf("Event time: %d\n", event_list[j].eventtime);
        }
        else
        {    printf("PID:: %d\n", event_list[0].job->pid);
            //j = 0;
            // Comprobamos si la casilla actual es más grande que el tiempo del evento para saber si va antes o despues. Ademas miramos si es la ultima casilla o no
            while (active_job[num_active_jobs].events[i].time_event - added_time >= 0 && j + 1 < num_event_list)
            {   
                // Esto Porque?? HACE FALTA EXPLICACIÓN
                //if (j != num_event_list - 1)
        
                j++; 
                added_time += event_list[j].eventtime;
              
            }
             printf("PID:: %d\n", event_list[0].job->pid);
            if (j + 1 < num_event_list)
            {
             
                for (int k = num_event_list; k > j; k--)
                {
                    event_list[k] = event_list[k - 1];
                }

                event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;// + event_list[j + 1].eventtime;
                added_time += event_list[j].eventtime;
                event_list[j + 1].eventtime -= event_list[j].eventtime;
                event_list[j].job = &active_job[num_active_jobs];
                event_list[j].eventnum = i;
                printf("J: %d\n", j);
                printf("PID j: %d\n", event_list[j].job->pid);
               // printf("PID j-1: %d\n", event_list[j-1].job->pid);
                printf("PID j+1: %d\n", event_list[j+1].job->pid);
                
            }
            else
            {
                if(active_job[num_active_jobs].events[i].time_event - added_time < 0){
          
                    event_list[j+1] = event_list[j];
                    event_list[j].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;
               
                    added_time += event_list[j].eventtime;
                    event_list[j].job = &active_job[num_active_jobs];
                    event_list[j].eventnum = i;
                }else{
                     printf("J: %d\n", j);
                printf("PID j: %d\n", event_list[j].job->pid);
                printf("PID:: %d\n", event_list[0].job->pid);
              //  printf("PID j-1: %d\n", event_list[j-1].job->pid);
               // printf("PID j+1: %d\n", event_list[j+1].job->pid);
                    printf("dsufjisujdibdufjhsdfujhndsujfgb\n");
                    event_list[j+1].eventtime = active_job[num_active_jobs].events[i].time_event - added_time;
                
                    added_time += event_list[j+1].eventtime;
                    event_list[j + 1].job = &active_job[num_active_jobs];
                    event_list[j + 1].eventnum = i;
                }
                 printf("J: %d\n", j);
                printf("PID j: %d\n", event_list[j].job->pid);
              //  printf("PID j-1: %d\n", event_list[j-1].job->pid);
                printf("PID j+1: %d\n", event_list[j+1].job->pid);
            }
            
            j++;
            //event_list[j].job = &active_job[num_active_jobs];
            //event_list[j].eventnum = i;
            

            if (event_list[j].eventtime < 0)
            {
                printf("ERROR NUMERO NEGATIVO EN TIEMPO DE EVENTO: Job id %d; Evento: %d; Job time: %d\n", event_list[j].job->pid, i, event_list[j].eventtime);
                exit(1);
            }
        }
        num_event_list++;
    }

    num_active_jobs++;

    /*if (num_event_list != num_active_jobs)
    {
        printf("ERROR: Event list and active jobs are not the same size\n");
        printf("Event list: %d\n", num_event_list);
        printf("Active jobs: %d\n", num_active_jobs);
        print_info();
        exit(1);
    }*/

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
    printf("Free cores: %d\n", free_cores);

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

int scheduler()
{
    while (1)
    {

        if (num_jobs != 0 && job_queue[0].arrival_time <= ciclototal && job_queue[0].events[0].num_cores <= free_cores)
        {
            insert_job(&job_queue[0]);
        }

        denb++;

        checkevent();

        if (num_jobs == 0 && num_active_jobs <= 0)
        {   printf("Num active jobs: %d\n", num_active_jobs);
            printf("All jobs completed\n");
            break;
        }

        print_info();

        ciclototal++;
       // usleep(100000);
    }

    return 0;
}