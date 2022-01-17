#include <pthread.h>
#include <stdlib.h>
#include "job.h"
#include "threads.h"
#include "queue.h"


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
	// semaphore up
	Job *j = queue_pop(sch);
	

}