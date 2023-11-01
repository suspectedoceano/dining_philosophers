#include "philo.h"

/*
 * INPUT
 *
 * ./philo 5 800 200 200 [7]
*/
int	main(int ac, char **av)
{
	t_table	table;

	if (5 == ac || 6 == ac)
	{
		parse_input(&table, av);
		data_init(&table);
		dinner_start(&table);
		clean(&table);
	}
	else
	{
		error_exit("Wrong input:\n"
			G"✅ ./philo 5 800 200 200 [7] ✅\n"
			"         t_die t_eat t_sleep [meals_limit]"RST);
	}
}
