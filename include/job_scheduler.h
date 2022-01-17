typedef (void *)(*RoutineFunc)(Pointer arguments);

#include "common_types.h"
#include "core.h"

struct Job{
	RoutineFunc routine;	//code to execute
	Pointer arguments;	//pointer to the corresponding arguments struct
}


struct DocArgs{		//MatchDocument arguments
	char *str;
	DocID id;
}

struct SQArgs{		//StartQuery arguments
	char *str;
	QueryID id;
	MatchType match_type;
	int match_distance;
}

struct EQArgs{		//EndQuery arguments
	QueryID id;
}

typedef struct JobScheduler{
	int execution_threads; 		// number of execution threads
	Queue q; 					// a queue that holds submitted jobs / tasks
	p_thread_t* tids; 			// execution threads
								// mutex, condition variable, ...
}JobScheduler;

JobScheduler* initialize_scheduler(int execution_threads);
int submit_job(JobScheduler* sch, Job* j);
int execute_all_jobs(JobScheduler* sch);