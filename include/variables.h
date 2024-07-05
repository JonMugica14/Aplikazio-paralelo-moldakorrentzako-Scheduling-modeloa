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
};

extern struct job *job_queue;
extern int num_jobs;

struct cores
{
    int id;
    struct job job;
    int busy;
    int time_core;
};

extern struct cores *cores;
extern int num_cores;
extern int Time;
extern int free_cores;

struct scheduler_events
{
    struct job *jobs;
    int num_jobs;
    int time_scheduler_event;
};

extern struct scheduler_events *scheduler_events;
extern int num_scheduler_events;