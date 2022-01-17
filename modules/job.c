#include <stdlib.h>

#include "job.h"

Job* job_create(RoutineFunc routine, Pointer arguments) {
    Job* job = malloc(sizeof(*job));
    job->routine = routine;
    job->arguments = arguments;

    return job;
}

void job_destroy(Job* job) {
    free(job);
}