#include "codexion.h"

void *monitor_thread(void *arg)
{
	int				i;
	int				j;
	int				all_done;
	t_simulation	*sim;

	i = 0;
	j = 0;
	all_done = 1;
	sim = (t_simulation *)arg;
	while(1)
	{
		while(i < sim->n_coders)
		{
			if (get_time_ms() - sim->coders[i].timestamp_lastcompile > sim->time_to_burnout)
			{
				pthread_mutex_lock(&sim->log_mutex);
				printf("%ld %d burned out\n", (get_time_ms() - sim->start_time), sim->coders[i].coder_id);
				pthread_mutex_unlock(&sim->log_mutex);
				pthread_mutex_lock(&sim->stop_mutex);
				sim->simulation_done = 1;
				pthread_cond_broadcast(&sim->stop_cond);
				pthread_mutex_unlock(&sim->stop_mutex);
				return (NULL);
			}
			i++;
		}
		all_done = 1;
		while(j < sim->n_coders)
		{
			if (sim->coders[j].compile_count < sim->number_of_compiles_required)
			{
				all_done = 0;
			}
			j++;
		}
		i = 0;
		j = 0;
		printf("counts: %d %d %d, required: %d\n",
		sim->coders[0].compile_count,
		sim->coders[1].compile_count,
		sim->coders[2].compile_count,
		sim->number_of_compiles_required);
		if (all_done)
		{
			pthread_mutex_lock(&sim->stop_mutex);
			sim->simulation_done = 1;
			pthread_cond_broadcast(&sim->stop_cond);
			pthread_mutex_unlock(&sim->stop_mutex);
			return (NULL);
		}
		usleep(1000);
	}
}
