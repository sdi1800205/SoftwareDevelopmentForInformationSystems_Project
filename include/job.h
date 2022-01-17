#pragma once

#include "common_types.h"
#include "core.h"

typedef struct Job{
	RoutineFunc routine;	//code to execute
	Pointer arguments;	//pointer to the corresponding arguments struct
}Job;


typedef struct DocArgs{		//MatchDocument arguments
	DocID id;
	char* str;
}DocArgs;

typedef struct SQArgs{		//StartQuery arguments
	QueryID id;
	char* str;
	MatchType match_type;
	int match_distance;
}SQArgs;

typedef struct EQArgs{		//EndQuery arguments
	QueryID id;
}EQArgs;

// Job functions
Job* job_create(RoutineFunc routine, Pointer arguments);
void job_destroy(Job* job);