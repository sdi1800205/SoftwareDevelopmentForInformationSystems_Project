#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "threads.h"
#include "job_scheduler.h"
#include "job.h"
#include "Queue.h"

extern JobScheduler* JobSch;
extern int stop_threads, can_exec, can_continue;
int stop_wait=0;

extern int threads_passed;

Job* take_job() {
	pthread_mutex_lock(&(JobSch->mtx_read));
	Job *job = queue_pop(JobSch->queue);
	pthread_mutex_unlock(&(JobSch->mtx_read));

	return job;
}

void* start_routine(void* arg){
	printf("Threads running %d\n",JobSch->execution_threads);
	do {
		printf("Thread %lu started!\n",pthread_self());
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
		printf("Thread %lu reached barrier wait\n",pthread_self());
		pthread_barrier_wait(&(JobSch->barrier));
		can_exec = 0;
		printf("Thread %lu after barrier!\n",pthread_self());

		pthread_mutex_lock(&(JobSch->mtx_threads_passed));
		if (++threads_passed == JobSch->execution_threads) {
			threads_passed = 0;

			stop_wait = 1;
			pthread_cond_signal(&(JobSch->cond_end_exec));
		}
		pthread_mutex_unlock(&(JobSch->mtx_threads_passed));

	} while(!stop_threads);

	printf("Thread %lu finished!,stop_threads %d\n",pthread_self(),stop_threads);
	
	return NULL;
}