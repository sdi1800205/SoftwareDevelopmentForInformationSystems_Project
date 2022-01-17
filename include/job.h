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


typedef (void *)(*RoutineFunc)(Pointer arguments);