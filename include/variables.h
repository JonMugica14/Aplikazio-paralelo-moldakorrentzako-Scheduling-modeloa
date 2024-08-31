#include <stdint.h>

// Evento que tendra cada trabajo. Señala cuando sucedera (respecto al tiempo de inicio del trabajo) y a cuantos cores cambiará
struct event
{
    int time_event;
    int num_cores;
};

// Trabajo
struct job
{
    int pid;
    int arrival_time;
    struct event *events; // Listado de eventos
    int num_events;
    struct cores *cores; // Cores actuales que estan ejecutando este trabajo
    int num_cores;
};

extern struct job *job_queue; // Listado de trabajos no inicializados, el SLURM los tiene que meter a ejecutar
extern int num_jobs;
extern struct job *active_job; // Listado de trabajos iniciados
extern int num_active_jobs;

// Core
struct cores
{
    int id;
    int busy;
    struct job job;
};

extern struct cores *cores; // Listado de cores
extern int max_cores;       // Numero Máximo de cores
extern int denb;            // Tiempo desde la última inserción de jobs
extern int ciclototal;     // Ciclo total de ejecución
extern int free_cores;      // Numero de cores libres
extern int blocked;          //Señaliza si hay algun trabajo bloqueado

struct scheduled_events 
{
    struct job *job;    // El trabajo
    int eventnum;       // El numero del evento del trabajo
    int eventtime;      // El tiempo que falta para ejecutarlo (Comparado con el anterior)
};

extern struct scheduled_events *event_list; // Lista de eventos a futuro
extern int num_event_list;
extern int ema;