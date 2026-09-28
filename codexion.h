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
	|	└── int main(int argc, char **argv)
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
	|	└── long get_time_ms(void)
	|
	├── coder.c
	|	└── void *coder_thread(void *arg)
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
