#include "codexion.h"

int main(int argc, char **argv)
{
	t_simulation	simulation;

	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments\n");
		return (1);
	}
	if (parse_main(argv, &simulation))
		return (1);
	if (init_simulation(&simulation))
	{
		fprintf(stderr, "Error: initialization failed\n");
		return (1);
	}
	if (start_threads(&simulation))
	{
		fprintf(stderr, "Error: Thread creation failed\n");
		return (1);
	}
	stop_threads(&simulation);
	cleanup(&simulation, simulation.n_coders);

	return (0);
}
