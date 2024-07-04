#include <stdint.h>

struct event
{
    int time;
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
    int time;
};

extern struct cores *cores;
extern int num_cores;
extern int time;
extern int free_cores;

struct scheduler_events
{
    struct job *jobs;
    int num_jobs;
    int time;
};

extern struct scheduler_events *scheduler_events;
extern int num_scheduler_events;