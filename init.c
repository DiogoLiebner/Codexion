#include "codexion.h"

void cleanup(t_simulation *sim, int initialized_dongles)
{
	int i;
	
	i = 0;
	while (i < initialized_dongles)
	{
		pthread_mutex_destroy(&sim->dongles[i].dongle_state);
		pthread_cond_destroy(&sim->dongles[i].cond_var);
		free(sim->dongles[i].wait_queue);
		i++;
	}
	free(sim->coders);
	free(sim->dongles);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->stop_mutex);
	pthread_cond_destroy(&sim->stop_cond);
}

static int init_dongles(t_simulation *sim, t_dongle *dongle, int id)
{
	if (pthread_mutex_init(&dongle->dongle_state, NULL) != 0)
		return (1);
	if (pthread_cond_init(&dongle->cond_var, NULL) != 0)
	{
		pthread_mutex_destroy(&dongle->dongle_state);
		return (1);
	}
	dongle->wait_queue = malloc(sizeof(t_queue_entry) * sim->n_coders);
	if (!dongle->wait_queue)
	{
		pthread_mutex_destroy(&dongle->dongle_state);
		pthread_cond_destroy(&dongle->cond_var);
		return (1);
	}
	dongle->dongle_id = id;
	dongle->is_available = 1;
	dongle->queue_size = 0;
	dongle->timestamp_released = 0;
	return (0);
}

static int init_coders(t_simulation *sim, t_coder *coder, int id)
{
	coder->left_dongle = &sim->dongles[id];
	coder->right_dongle = &sim->dongles[(id + 1) % sim->n_coders];
	coder->coder_id = id + 1;
	coder->timestamp_lastcompile = 0;
	coder->compile_count = 0;
	coder->sim = sim;
	return (0);
}

int init_simulation(t_simulation *sim)
{
	int	i;

	i = 0;
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->stop_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		return (1);
	}
	if (pthread_cond_init(&sim->stop_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		return (1);
	}
	sim->coders = malloc(sizeof(t_coder) * sim->n_coders);
	if (!sim->coders)
	{
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_cond_destroy(&sim->stop_cond);
		return (1);
	}
	sim->dongles = malloc(sizeof(t_dongle) * sim->n_coders);
	if (!sim->dongles)
    {
		pthread_mutex_destroy(&sim->log_mutex);
		pthread_mutex_destroy(&sim->stop_mutex);
		pthread_cond_destroy(&sim->stop_cond);
        free(sim->coders);
		return (1);
    }
	sim->simulation_done = 0;
	while (i < sim->n_coders)
	{
		if (init_dongles(sim, &sim->dongles[i], i))
		{
			cleanup(sim, i);
			return (1);
		}
		init_coders(sim, &sim->coders[i], i);
		i++;
	}
	return (0);
}
