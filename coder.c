/*
coder.c
	├── void *coder_thread(void *arg)
*/

#include "codexion.h"

void *coder_thread(void *arg)
{
	t_coder *coder;
	int done;
	
	coder = (t_coder *)arg;
	while (1)
	{
		pthread_mutex_lock(&coder->sim->stop_mutex);
		done = coder->sim->simulation_done;
		pthread_mutex_unlock(&coder->sim->stop_mutex);
		if (done)
			break;
		if (coder->coder_id % 2 == 0)
		{
			acquire_dongle(coder, coder->right_dongle);
			if (coder->interrupted)
				return (NULL);
			pthread_mutex_lock(&coder->sim->log_mutex);
			printf("%ld %d has taken a dongle\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
			pthread_mutex_unlock(&coder->sim->log_mutex);
			acquire_dongle(coder, coder->left_dongle);
			if (coder->interrupted)
				return (NULL);
			pthread_mutex_lock(&coder->sim->log_mutex);
			printf("%ld %d has taken a dongle\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
			pthread_mutex_unlock(&coder->sim->log_mutex);
		}
		else
		{
			acquire_dongle(coder, coder->left_dongle);
			if (coder->interrupted)
				return (NULL);
			pthread_mutex_lock(&coder->sim->log_mutex);
			printf("%ld %d has taken a dongle\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
			pthread_mutex_unlock(&coder->sim->log_mutex);
			acquire_dongle(coder, coder->right_dongle);
			if (coder->interrupted)
				return (NULL);
			pthread_mutex_lock(&coder->sim->log_mutex);
			printf("%ld %d has taken a dongle\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
			pthread_mutex_unlock(&coder->sim->log_mutex);
		}
		coder->timestamp_lastcompile = get_time_ms();
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is compiling\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_compile * 1000);
		release_dongle(coder->left_dongle);
		release_dongle(coder->right_dongle);
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is debugging\n",  (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_debug * 1000);
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is refactoring\n",  (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_refactor * 1000);
		coder->compile_count++;
	}
	return (NULL);
}
