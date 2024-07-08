#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "../include/variables.h"
#include "../include/Scheduler.h"

int core()
{
   

        for (int i = 0; i < num_active_jobs; i++)
        {
            if (active_job[i].events[0].time_event == 0)
            {
                event_list[num_event_list] = active_job[i];
                num_event_list++;
            }
            else
            {
                active_job[i].events[0].time_event--;
            }
        }



     
    
    return 0;
}