#include <stdlib.h>
#include <stdio.h>
#include "job_scheduler.h"
#include "threads.h"

int stop_threads = 0,can_exec = 0, finish = 0;
extern int stop_wait;

int threads_passed = 0;

JobScheduler* initialize_scheduler(int execution_threads){
	JobScheduler *sch = (JobScheduler *)malloc(sizeof(JobScheduler));
	
	sch->queue = queue_create();
	sch->execution_threads = execution_threads;
	sch->docs = false;		//initialise bool docs variable to false, because first jobs to be executed will be start queries

	pthread_cond_init(&(sch->cond_start_exec),NULL);
	pthread_cond_init(&(sch->cond_end_exec),NULL);

	pthread_mutex_init(&(sch->mtx_start_exec),NULL);
	pthread_mutex_init(&(sch->mtx_end_exec),NULL);

	pthread_mutex_init(&(sch->mtx_read),NULL);
	pthread_mutex_init(&(sch->mtx_threads_passed),NULL);

	pthread_barrier_init(&(sch->barrier),NULL,sch->execution_threads);

	sch->tids = (pthread_t *)malloc(sizeof(pthread_t)*execution_threads);		//array containing the IDs of the created threads

	return sch;
}

int submit_job(JobScheduler* sch, Job* j){
	pthread_mutex_lock(&(sch->mtx_read));
	queue_push(sch->queue, j);
	pthread_mutex_unlock(&(sch->mtx_read));

	return 0;
}

int execute_all_jobs(JobScheduler* sch){
	can_exec = 1;			//indicate that it's safe to continue after pthread_cond_wait(&(sch->cond_start_exec),&(sch->mtx_start_exec))
    pthread_cond_broadcast(&(sch->cond_start_exec));

	stop_wait = 0;

    return 0;
}

int wait_all_tasks_finish(JobScheduler* sch){
	pthread_mutex_lock(&(sch->mtx_end_exec));
	while(!stop_wait && !finish) {
		pthread_cond_wait(&(sch->cond_end_exec),&(sch->mtx_end_exec));
	}
	pthread_mutex_unlock(&(sch->mtx_end_exec));
	
	can_exec = 0;

	return 0;
}

int destroy_scheduler(JobScheduler* sch){

	stop_threads = 1;
	execute_all_jobs(sch);
	wait_all_tasks_finish(sch);

	for(int i=0; i<sch->execution_threads; i++){
		pthread_join(sch->tids[i],0);
	}
	free(sch->tids);

	pthread_cond_destroy(&(sch->cond_start_exec));
	pthread_cond_destroy(&(sch->cond_end_exec));

	pthread_mutex_destroy(&(sch->mtx_start_exec));
	pthread_mutex_destroy(&(sch->mtx_end_exec));

	pthread_mutex_destroy(&(sch->mtx_read));
	pthread_mutex_destroy(&(sch->mtx_threads_passed));

	pthread_barrier_destroy(&(sch->barrier));

	queue_destroy(sch->queue);

	free(sch);

	return 0;
}

////////////////////// ectra functions /////////////////////////
int jobscheduler_size(JobScheduler* sch) {
	return queue_size(sch->queue);
}