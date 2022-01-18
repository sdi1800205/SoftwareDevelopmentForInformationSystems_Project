#include <pthread.h>
#include <stdlib.h>

#include "job_scheduler.h"
#include "job.h"

extern JobScheduler* JobSch;

Job* take_job() {
	
}

void start_routine(void){

	while(!stop_threads){

		pthread_mutex_lock(&mtx_start_exec);
		while(!can_exec) {
			pthread_cond_wait(&cond_start_exec, &mtx_start_exec);
		}

		if(stop_threads)
			break;

		while(1){

			Job* job = take_job();
			if(job != NULL)
				(job->routine)(job->arguments);
			else
				break;

		}
		// pthread_mutex_unlock(&mtx_start_exec)

		pthread_cond_signal(&cond_end_exec);

	}
}
