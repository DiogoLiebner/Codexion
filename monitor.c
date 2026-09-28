#include "codexion.h"

void *monitor_thread(void *arg)
{
	int				i;
	int				j;
    int             k;
	int				all_done;
	t_simulation	*sim;

	i = 0;
	j = 0;
    k = 0;
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
                k = 0;
                while (k < sim->n_coders)
                {
                    pthread_mutex_lock(&sim->dongles[k].dongle_state);
                    pthread_cond_broadcast(&sim->dongles[k].cond_var);
                    pthread_mutex_unlock(&sim->dongles[k].dongle_state);
                    k++;
                }
				return (NULL);
			}
			i++;
		}

        j = 0;
		all_done = 1;
		while(j < sim->n_coders)
		{
            pthread_mutex_lock(&sim->stop_mutex);
			if (sim->coders[j].compile_count < sim->number_of_compiles_required)
			{
				all_done = 0;
			}
            pthread_mutex_unlock(&sim->stop_mutex);
			j++;
		}
		i = 0;
		if (all_done)
		{
            printf("ALL DONE TRIGGERED\n");
            int c = 0;
            while (c < sim->n_coders)
            {
                printf("coder %d has compiled %d times\n", sim->coders[c].coder_id, sim->coders[c].compile_count);
                c++;
            }
			pthread_mutex_lock(&sim->stop_mutex);
			sim->simulation_done = 1;
			pthread_cond_broadcast(&sim->stop_cond);
			pthread_mutex_unlock(&sim->stop_mutex);
            k = 0;
            while (k < sim->n_coders)
            {
                pthread_mutex_lock(&sim->dongles[k].dongle_state);
                pthread_cond_broadcast(&sim->dongles[k].cond_var);
                pthread_mutex_unlock(&sim->dongles[k].dongle_state);
                k++;
            }
			return (NULL);
		}
		usleep(1000);
	}
}
