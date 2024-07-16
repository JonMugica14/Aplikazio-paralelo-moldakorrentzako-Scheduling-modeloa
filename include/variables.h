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
extern int free_cores;      // Numero de cores libres

extern struct job *event_list; // Lista de eventos que han saltado
extern int num_event_list;
