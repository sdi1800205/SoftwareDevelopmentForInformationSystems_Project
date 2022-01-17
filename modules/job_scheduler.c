#include <pthread.h>
#include <stdlib.h>
#include "job.h"
#include "threads.h"
#include "Queue.h"

JobScheduler* initialize_scheduler(int execution_threads){
	JobScheduler *sch = (JobScheduler *)malloc(sizeof(JobScheduler));
	sch->tids = (pthread_t *)malloc(sizeof(pthread_t)*execution_threads);		//array containing the IDs of the created threads
	for(int i=0; i<execution_threads; i++){
		if(pthread_create(&tids[i], NULL , start_routine, NULL) < 0){
			exit(EXIT_FAILURE);
		}
	}

	sch->q = queue_create();
	sch->execution_threads = execution_threads;

	

	return sch;
}

int submit_job(JobScheduler* sch, Job* j){
	queue_push(sch->q, j);
	
	return 1;
}

int execute_all_jobs(JobScheduler* sch){
	can_exec = 1;			//indicate that it's safe to continue after pthread_cond_wait(&(sch->cond_start_exec),&(sch->mtx_start_exec))
    pthread_cond_broadcast(&(sch->cond_start_exec),&(sch->mtx_start_exec));
}

int wait_all_tasks_finish(JobScheduler* sch){
	for(int i=0; i<sch->execution_threads; i++){
		pthread_cond_wait(&(sch->cond_end_exec),&(sch->mtx_end_exec));
	}
	can_exec = 0;		//reset variable
}

int destroy_scheduler(JobScheduler* sch){
	stop_threads = 1;

	pthread_cond_broadcast(&(sch->cond_start_exec),&(sch->mtx_start_exec));
	for(int i=0;)
	queue_destroy(sch->q);

}