#include "common_types.h"
#include "core.h"

typedef struct JobScheduler{
	int execution_threads; 		// number of execution threads
	Queue q; 					// a queue that holds submitted jobs / tasks
	p_thread_t* tids; 			// execution threads
								// mutex, condition variable, ...
}JobScheduler;

JobScheduler* initialize_scheduler(int execution_threads);
int submit_job(JobScheduler* sch, Job* j);
int execute_all_jobs(JobScheduler* sch);