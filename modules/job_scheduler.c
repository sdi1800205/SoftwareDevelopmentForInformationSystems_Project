#include <stdlib.h>
#include "job_scheduler.h"

int stop_threads = 0,can_exec = 0;

JobScheduler* initialize_scheduler(int execution_threads){
	JobScheduler *sch = (JobScheduler *)malloc(sizeof(JobScheduler));
	
	sch->q = queue_create();
	sch->execution_threads = execution_threads;

	pthread_cond_init(&(sch->cond_start_exec),0);
	pthread_cond_init(&(sch->cond_end_exec),0);

	pthread_mutex_init(&(sch->mtx_start_exec),0);
	pthread_mutex_init(&(sch->mtx_end_exec),0);

	pthread_mutex_init(&(sch->mtx_read),0);

	pthread_barrier_init(&(sch->barrier),0,sch->execution_threads);

	sch->tids = (pthread_t *)malloc(sizeof(pthread_t)*execution_threads);		//array containing the IDs of the created threads
	for(int i=0; i<execution_threads; i++){
		if(pthread_create(&tids[i], NULL , start_routine, NULL) < 0){
			exit(EXIT_FAILURE);
		}
	}

	return sch;
}

int submit_job(JobScheduler* sch, Job* j){
	pthread_mutex_lock(&(sch->mtx_read));
	queue_push(sch->q, j);
	pthread_mutex_unlock(&(sch->mtx_read));

	return 1;
}

int execute_all_jobs(JobScheduler* sch){
	can_exec = 1;			//indicate that it's safe to continue after pthread_cond_wait(&(sch->cond_start_exec),&(sch->mtx_start_exec))
    pthread_cond_broadcast(&(sch->cond_start_exec),&(sch->mtx_start_exec));

    return 0;
}

int wait_all_tasks_finish(JobScheduler* sch){
	pthread_cond_wait(&(sch->cond_end_exec),&(sch->mtx_end_exec));
	can_exec = 0;		//reset variable

	return 0;
}

int destroy_scheduler(JobScheduler* sch){
	stop_threads = 1;
	execute_all_jobs(sch);

	for(int i=0; i<execution_threads; i++){
		pthread_join(tids[i],0);
	}

	pthread_cond_destroy(&(sch->cond_start_exec));
	pthread_cond_destroy(&(sch->cond_end_exec));

	pthread_mutex_destroy(&(sch->mtx_start_exec));
	pthread_mutex_destroy(&(sch->mtx_end_exec));

	pthread_mutex_destroy(&(sch->mtx_read));

	pthread_barrier_destroy(&(sch->barrier));

	queue_destroy(sch->q);

	free(sch);
}