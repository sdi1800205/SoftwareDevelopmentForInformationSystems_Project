///threads.c

void start_routine(void){

	while(!stop_threads){

		do{
			pthread_cond_wait(cond_exec)
		}while(!can_exec);

		if(stop_threads)
			break;

		while(1){

			job = take_job();
			if(job != NULL)
				(job->routine)(job->args);
			else
				break;

		}

		signal(cond_end_exec);

	}

}