// Coder Struct
// - Coder ID   
// - reference to left dongle
// - reference to right dongle
// - timestamp of last compile start (for burnout detection)
// - compile count (how many times they have compiled)

// Dongle Struct
// - Dongle ID
// - is it being held or not (bool)
// - timestamp of when it was released (cooldown)
// - wait queue/ priority of coders waiting for it 
// - mutex (protects dongle state)
// - condition variable (for waiting coders)

// Simulation / shared state struct
// - total number of coders
// - time_to_burnout
// - time_to_compile
// - time_to_debug
// - time_to_refactor
// - number_of_compiles_required
// - dongle_cooldown
// - scheduler type (fifo or edf)
// - simulation done (bool)
// - stop condition variable (for waking sleeping threads)
// - stop mutex (paired with stop condition variable)
// - log mutex (serializes all print output)
// - array of coders
// - array of dongles

// Priority queue entry struct
// - coder ID
// - requested timestamp (for FIFO)
// - deadline timestamp (for edf)

// Priority queue struct
// - array of entries (heap)
// - current size
// - scheduler type (to know which comparison to use)

// Implementation Incremental tests
// Step 1 — valid args print correctly, invalid args (negative numbers, non-integers, wrong scheduler string) exit with error
// Step 2 — single coder prints compile/debug/refactor loop correctly with right timestamps
// Step 3 — multiple coders run simultaneously, observe race conditions and potential deadlock
// Step 4 — race conditions gone, no interleaved log output, dongle access is exclusive
// Step 5 — dongle is unavailable for correct cooldown duration after release
// Step 6 — FIFO queue serves coders in arrival order
// Step 7 — EDF serves coder with earliest deadline first, tie broken by lower coder ID
// Step 8 — burnout logged within 10ms, all coder threads stop cleanly
// Step 9 — 1 coder edge case, very short burnout times, very long cooldowns, large N coders, FIFO vs EDF same parameters

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <pthread.h>
# include <string.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

/*
	├── codexion.c
	|	├── int main(int argc, char **argv)
	|
	├── parsing.c
    |	├── int parse_main(char **argv, t_simulation *sim)
    |	├── static int parse_scheduler(char *str)
    |	└── static int is_valid_integer(char *str)
	|
	├── init.c
	|	├── static int init_coders(t_simulation *sim, t_coder *coder, int id)
	|	├── static int init_dongles(t_simulation *sim, t_dongle *dongle, int id)
	|	├── int init_simulation(t_simulation *sim)
	|	└── void cleanup(t_simulation *sim, int initialized_dongles)
	|
	├── utils.c
	|	├── long get_time_ms(void)
	|
	├── coder.c
	|	├── void *coder_thread(void *arg)
	|	├── 
	|	├── 
    |
    ├── dongle.c
    |   ├── void acquire_dongle(t_coder *coder, t_dongle *dongle)
    |   └── void release_dongle(t_coder *coder, t_dongle *dongle)
	|
    ├── threads.c
    |   ├── int start_threads(t_simulation *sim)
    |   └── void stop_threads(t_simulation *sim)
	|
	├── scheduler.c
    |   ├── int entry_compare(t_queue_entry *entry1, t_queue_entry *entry2, int scd_type)
    |   ├── static void bubble_up(t_queue_entry *queue, int size, int scd_type)
	|   ├── static void bubble_down(t_queue_entry *queue, int size, int scd_type)
	|   ├── void heap_insert(t_dongle *dongle, t_queue_entry entry, int scd_type)
	|   ├── t_queue_entry heap_pop(t_dongle *dongle, int scd_type)
	|	└── t_queue_entry heap_peek(t_dongle *dongle)
*/

typedef struct s_simulation t_simulation;

typedef struct s_queue_entry{
	int		coder_id;
	long	request_time;
	long	deadline;
}	t_queue_entry;

typedef struct s_dongle{
	int 			dongle_id;
	int 			is_available;
	long			timestamp_released;
	t_queue_entry	*wait_queue;
	int				queue_size;
	pthread_mutex_t dongle_state;
	pthread_cond_t	cond_var;
}	t_dongle;

typedef struct s_coder{
	int				coder_id;
	int				compile_count;
	int				interrupted;
	long			timestamp_lastcompile;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_simulation	*sim;
}	t_coder;

typedef struct s_simulation{
	int				n_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	int				scheduler;
	int		 		simulation_done;
    long            start_time;
    pthread_t       monitor;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	stop_mutex;
	pthread_cond_t	stop_cond;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_simulation; 

int				parse_main(char **argv, t_simulation *sim);

int				init_simulation(t_simulation *sim);
void			cleanup(t_simulation *sim, int initialized_dongles);

long			get_time_ms(void);

void			*coder_thread(void *arg);

int				start_threads(t_simulation *sim);
void			stop_threads(t_simulation *sim);

void			acquire_dongle(t_coder *coder, t_dongle *dongle);
void			release_dongle(t_dongle *dongle);

int				entry_compare(t_queue_entry *entry1, t_queue_entry *entry2, int scd_type);
void			heap_insert(t_dongle *dongle, t_queue_entry entry, int scd_type);
t_queue_entry	heap_pop(t_dongle *dongle, int scd_type);
t_queue_entry	heap_peek(t_dongle *dongle);

void            *monitor_thread(void *arg);

#endif
