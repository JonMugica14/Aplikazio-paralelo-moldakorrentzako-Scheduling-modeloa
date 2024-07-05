#include <stdint.h>

struct event
{
    int time_event;
    int num_cores;
};

struct job
{
    int pid;
    int arrival_time;
    struct event *events;
    int num_events;
    struct cores *cores;
    int num_cores;
};

extern struct job *job_queue;
extern int num_jobs;
extern struct job *active_job;
extern int num_active_jobs;

struct cores
{
    int id;
    int busy;
    //int time_core;
    //struct job job;
};

extern struct cores *cores;
extern int num_active_cores;
extern int max_cores;
extern int denb;
extern int free_cores;

extern struct job *event_list;
extern int num_event_list;
