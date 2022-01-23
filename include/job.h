#pragma once

#include "common_types.h"
#include "core.h"

typedef struct Job{
	RoutineFunc routine;	// code to execute
	Pointer arguments;		// pointer to the corresponding arguments struct
	char type;				// variable that holds the type of job(start/end query or match document)
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
Job* job_create(RoutineFunc routine, Pointer arguments, char job_type);
void job_destroy(Job* job);

// argument structs' functions
DocArgs* docargs_create(DocID id, char* str);
SQArgs* sqargs_create(QueryID id, char* str, MatchType match_type, int match_dist);
EQArgs* eqargs_create(QueryID id);

void docargs_destroy(DocArgs* args);
void sqargs_destroy(SQArgs* args);
void eqargs_destroy(EQArgs* args);