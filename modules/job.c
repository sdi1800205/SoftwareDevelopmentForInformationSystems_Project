#include <stdlib.h>

#include "job.h"

Job* job_create(RoutineFunc routine, Pointer arguments) {
    Job* job = malloc(sizeof(*job));
    job->routine = routine;
    job->arguments = arguments;

    return job;
}

void job_destroy(Job* job) {
    docargs_destroy(job->arguments);
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