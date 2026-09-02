#include "codexion"

int main(int argc, char **argv)
{
	t_simulation	simulation;

	if (argc != 9)
	{
		fprintf(stderr, "Error: Invalid number of arguments\n");
		return (1);
	}
	else
		if (parse_main(argv, &simulation))
			return (1);
		if (init_simulation(&simulation))
		{
			fprintf(stderr, "Error: initializatino failed\n");
			return (1);
		}
	return (0);
}
