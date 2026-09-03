/*
dongle.c
	├── void acquire_dongle(t_coder *coder, t_dongle *dongle)
	├── void release_dongle(t_coder *coder, t_dongle *dongle)
	├──
	└── 
*/

#include "codexion.h"

void acquire_dongle(t_coder *coder, t_dongle *dongle)
{
	t_queue_entry	entry;
	struct timespec ts;
	long wake_time;

	entry.coder_id = coder->coder_id;
	entry.request_time = get_time_ms();
	entry.deadline = coder->timestamp_lastcompile + coder->sim->time_to_burnout;

	pthread_mutex_lock(&dongle->dongle_state);
	heap_insert(dongle, entry, coder->sim->scheduler);
	while(!dongle->is_available
		|| get_time_ms() < dongle->timestamp_released + coder->sim->dongle_cooldown
		|| heap_peek(dongle).coder_id != coder->coder_id)
	{
		wake_time = dongle->timestamp_released + coder->sim->dongle_cooldown;
		ts.tv_sec = wake_time / 1000;
		ts.tv_nsec = (wake_time % 1000) * 1000000;
		pthread_cond_timedwait(&dongle->cond_var, &dongle->dongle_state, &ts);
	}
	heap_pop(dongle, coder->sim->scheduler);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->dongle_state);
}

void release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->dongle_state);
	dongle->is_available = 1;
	dongle->timestamp_released = get_time_ms();
	pthread_cond_broadcast(&dongle->cond_var);
	pthread_mutex_unlock(&dongle->dongle_state);
}
