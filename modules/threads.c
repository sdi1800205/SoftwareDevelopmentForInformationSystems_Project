#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "threads.h"
#include "job_scheduler.h"
#include "job.h"
#include "Queue.h"

extern JobScheduler* JobSch;
extern int stop_threads, can_exec;

Job* take_job() {
	pthread_mutex_lock(&(JobSch->mtx_read));
	Job *job = queue_pop(JobSch->queue);
	pthread_mutex_unlock(&(JobSch->mtx_read));

	return job;
}

void* start_routine(void* arg){

	while(!stop_threads){

		pthread_mutex_lock(&(JobSch->mtx_start_exec));
		while(!can_exec){
			pthread_cond_wait(&(JobSch->cond_start_exec) , &(JobSch->mtx_start_exec) );
		}
		pthread_mutex_unlock(&(JobSch->mtx_start_exec));

		printf("Thread %lu got in!,stop_threads %d\n",pthread_self(),stop_threads);
		

		while(1) {
			Job* job = take_job();
			if(job != NULL){
				(job->routine)(job->arguments);
				job_destroy(job);
			}
			else
				break;
		}

		pthread_barrier_wait (&(JobSch->barrier));
		pthread_cond_signal(&(JobSch->cond_end_exec));
	}

	printf("Thread %lu finished!,stop_threads %d\n",pthread_self(),stop_threads);
	
	return NULL;
}