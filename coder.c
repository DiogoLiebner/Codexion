#include "codexion.h"

void *coder_thread(void *arg)
{
	t_coder *coder;

	coder = (t_coder *)arg;
	while (!coder->sim->simulation_done)
	{
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is compiling\n", (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_compile * 1000);
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is debugging\n",  (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_debug * 1000);
		pthread_mutex_lock(&coder->sim->log_mutex);
		printf("%ld %d is refactoring\n",  (get_time_ms() - coder->sim->start_time), coder->coder_id);
		pthread_mutex_unlock(&coder->sim->log_mutex);
		usleep(coder->sim->time_to_refactor * 1000);
	}
	return (NULL);
}