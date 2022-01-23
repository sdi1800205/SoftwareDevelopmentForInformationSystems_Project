#include <stdlib.h>

#include "job.h"

Job* job_create(RoutineFunc routine, Pointer arguments,char job_type) {
    Job* job = malloc(sizeof(*job));
    job->routine = routine;
    job->arguments = arguments;
    job->type = job_type;

    return job;
}

void job_destroy(Job* job) {
    if(job->type == 'm'){
        docargs_destroy(job->arguments);
    }
    else if(job->type == 's'){

    }
    else{

    }
    free(job);
}

////////////////// argument structs' functions ////////////////////////
DocArgs* docargs_create(DocID id, char* str) {
    DocArgs* args = malloc(sizeof(*args));
    args->id = id;
    args->str = str;

    return args;
}

void docargs_destroy(DocArgs* args) {
    free(args->str);
    free(args);
}