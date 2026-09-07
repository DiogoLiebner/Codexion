#include "codexion.h"

int start_threads(t_simulation *sim)
{
	int i;
	int j;

	i = 0;
	j = -1;
	while (i < sim->n_coders)
	{
		if(pthread_create(&sim->coders[i].thread, NULL, coder_thread, &sim->coders[i]) != 0)
		{
			while (j >= 0)
			{
				pthread_cancel(sim->coders[j].thread);
				pthread_join(sim->coders[j].thread, NULL);
				j--;
			}
			return (1);
		}
		j = i;
		i++;
	}
    if (pthread_create(&sim->monitor, NULL, monitor_thread, sim) != 0)
    {
        return (1);
    }
	return (0);
}

void stop_threads(t_simulation *sim)
{
	int i;

	i = sim->n_coders - 1;
	while (i >= 0)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i--;
	}
    pthread_join(sim->monitor, NULL);
}