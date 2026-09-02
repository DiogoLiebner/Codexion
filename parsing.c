#include "codexion.h"

static int is_valid_integer(char *str)
{
	int i;

	i = 0;
	if (!str[i])
		return 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return 0;
		i++;	
	}
	return 1;
}

static int parse_scheduler(char *str)
{
	char *edf = "edf";
	char *fifo = "fifo";
	
	if (strcmp(str, fifo) == 0)
		return (0);
	else if (strcmp(str, edf) == 0)
		return (1);
	return (-1);
}

int parse_main(char **argv, t_simulation *sim)
{
	int i = 1;
	int scheduler = parse_scheduler(argv[8]);

	while (i < 8)
	{
		if (!(is_valid_integer(argv[i]) && atoi(argv[i]) > 0))
		{
			fprintf(stderr, "Error: invalid argument '%s'\n", argv[i]);
			return (1);
		}
		i++;
	}
	if (scheduler == -1){
		fprintf(stderr, "Error: scheduler must be 'fifo' or 'edf'\n");
		return (1);
	}
	sim->n_coders = atoi(argv[1]);
	sim->time_to_burnout = atoi(argv[2]);
	sim->time_to_compile = atoi(argv[3]);
	sim->time_to_debug = atoi(argv[4]);
	sim->time_to_refactor = atoi(argv[5]);
	sim->number_of_compiles_required = atoi(argv[6]);
	sim->dongle_cooldown = atoi(argv[7]);
	sim->scheduler = scheduler;
	return (0);
}
