#ifndef JOB_SCH
#define JOB_SCH

#include <pthread.h>

#include "common_types.h"
#include "core.h"
#include "Queue.h"
#include "job.h"

typedef struct JobScheduler{
	int execution_threads; 		// number of execution threads
	bool docs;				//indicates if scheduler is full of match documents or start/end queries

	Queue queue; 					// a queue that holds submitted jobs / tasks
	pthread_t* tids; 			// execution threads


	// mutex, condition variable, ...
	pthread_cond_t cond_start_exec;
	pthread_cond_t cond_end_exec;

	pthread_mutex_t mtx_start_exec;
	pthread_mutex_t mtx_end_exec;

	pthread_mutex_t mtx_read;

	pthread_mutex_t mtx_threads_passed;

	pthread_barrier_t barrier;
}JobScheduler;

JobScheduler* initialize_scheduler(int execution_threads);
int submit_job(JobScheduler* sch, Job* j);
int execute_all_jobs(JobScheduler* sch);
int wait_all_tasks_finish(JobScheduler* sch);
int destroy_scheduler(JobScheduler* sch);

// extra functions
int jobscheduler_size(JobScheduler* sch);

#endif