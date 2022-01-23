#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "threads.h"
#include "job_scheduler.h"
#include "job.h"
#include "Queue.h"

extern JobScheduler* JobSch;
extern int stop_threads, can_exec, finish;
int stop_wait=0;

extern int threads_passed;

Job* take_job() {
	// take priority to change the queue, pop a job and unlock for other threads
	pthread_mutex_lock(&(JobSch->mtx_read));
	Job *job = queue_pop(JobSch->queue);
	pthread_mutex_unlock(&(JobSch->mtx_read));

	return job;
}

void* start_routine(void* arg){
	do {
		// printf("Thread: %lu Started before lock!\n", pthread_self());
		// wait until parent thread send and signal to begin executing jobs
		// pthread_barrier_wait(&(JobSch->barrier));
		pthread_mutex_lock(&(JobSch->mtx_start_exec));
		// printf("Thread: %lu Started after lock!\n", pthread_self());
		while(!can_exec){
			pthread_cond_wait(&(JobSch->cond_start_exec) , &(JobSch->mtx_start_exec));
		}
		// printf("Thread: %lu after wait!\n", pthread_self());
		pthread_mutex_unlock(&(JobSch->mtx_start_exec));
		// printf("Thread: %lu continue!\n", pthread_self());

		// if parent ends the threads execution it means that he sent a signal to execute all jobs and this variable
		// will break the main loop of the function, as a result the threads execution
		if (stop_threads) {
			// only the last thread that ends sends a signal
			pthread_mutex_lock(&(JobSch->mtx_threads_passed));
			if (++threads_passed == JobSch->execution_threads) {
				finish = 1;			// indicate to the parent that the executions ended
				pthread_cond_signal(&(JobSch->cond_end_exec));
			}
			pthread_mutex_unlock(&(JobSch->mtx_threads_passed));
			break;
		}

		// loop in which the thread gets a job and executes it until no job remains in the queues
		while(1) {
			Job* job = take_job();
			if(job != NULL){						// case of an existing a job
				(job->routine)(job->arguments);
				job_destroy(job);
			}
			else									// case of all jobs have been executed
				break;
		}

		// after executing the jobs all the threads should end up at the same time so
		// we use barrier for this job 
		// printf("Thread: %lu reached barrier!\n", pthread_self());
		pthread_barrier_wait(&(JobSch->barrier));
		can_exec = 0;			// stop child threads from rerun, only parent thread permits that

		// the last thread will send a signal to the parent that every thread completed its execution so
		// he may proceed to his own work
		pthread_mutex_lock(&(JobSch->mtx_threads_passed));
		if (++threads_passed == JobSch->execution_threads) {
			threads_passed = 0;			// initialize count variable

			stop_wait = 1;				// let parent get off his wait loop
			pthread_cond_signal(&(JobSch->cond_end_exec));		// give the signal to parent to start
		}
		pthread_mutex_unlock(&(JobSch->mtx_threads_passed));

	} while(1);
	
	return NULL;
}