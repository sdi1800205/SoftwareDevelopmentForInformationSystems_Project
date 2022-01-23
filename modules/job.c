#include <stdlib.h>

#include "job.h"

Job* job_create(RoutineFunc routine, Pointer arguments, char job_type) {
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
        sqargs_destroy(job->arguments);
    }
    else{
        eqargs_destroy(job->arguments);
    }
    free(job);
}

////////////////// argument structs' functions ////////////////////////

///////////////////////// create ////////////////////////////////

DocArgs* docargs_create(DocID id, char* str) {
    DocArgs* args = malloc(sizeof(*args));
    args->id = id;
    args->str = str;

    return args;
}

SQArgs* sqargs_create(QueryID id, char* str, MatchType match_type, int match_dist) {
    SQArgs* args = malloc(sizeof(*args));
    args->id = id;
    args->str = str;
    args->match_type = match_type;
    args->match_distance = match_dist;

    return args;
}

EQArgs* eqargs_create(QueryID id) {
    EQArgs* args = malloc(sizeof(*args));
    args->id = id;

    return args;
}

///////////////////////// destroy ////////////////////////////////

void docargs_destroy(DocArgs* args) {
    free(args->str);
    free(args);
}

void sqargs_destroy(SQArgs* args) {
    free(args->str);
    free(args);
}

void eqargs_destroy(EQArgs* args) {
    free(args);
}