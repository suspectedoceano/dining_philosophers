#include "philo.h"
#include <stdlib.h>

/*
 * Returns time in milliseconds, microseconds,
 * hence scientific notation here is top
 *
 * return just to trick -Werror...
 * cause my error_exit will already...exit 😂
*/
long	gettime(int time_code)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		error_exit("Gettimeofday failed");
	if (MILLISECOND == time_code)
		return (tv.tv_sec * 1e3 + tv.tv_usec / 1e3);
	else if (MICROSECOND == time_code)
		return (tv.tv_sec * 1e6 + tv.tv_usec);
	else if (SECONDS == time_code)
		return (tv.tv_sec + tv.tv_usec / 1e6);
	else
		error_exit("Wrong input to gettime:"
			"use <MILLISECOND> <MICROSECOND> <SECONDS>");
	return (1337);
}

/*
 * HYBRID approach
 * given usleep is not precise
 * i usleep for majority of time ,
 * then refine wiht busy wait
*/
void	precise_usleep(long usec, t_table *table)
{
	long	start;
	long	elapsed;
	long	rem;

	start = gettime(MICROSECOND);
	while (gettime(MICROSECOND) - start < usec)
	{
		if (simulation_finished(table))
			break ;
		elapsed = gettime(MICROSECOND) - start;
		rem = usec - elapsed;
		if (rem > 1e3)
			usleep(rem / 2);
		else
			while (gettime(MICROSECOND) - start < usec)
				;
	}
}

/*
 * Avoid memory leaks
*/
void	clean(t_table *table)
{
	t_philo	*philo;
	int		i;

	i = -1;
	while (++i < table->philo_nbr)
	{
		philo = table->philos + i;
		safe_mutex_handle(&philo->philo_mutex, DESTROY);
	}
	safe_mutex_handle(&table->write_mutex, DESTROY);
	safe_mutex_handle(&table->table_mutex, DESTROY);
	free(table->forks);
	free(table->philos);
}

/*
 * Print a custom error message and exit
 * Output is modeled by ANSI sequences
 * R->RED
 * RST->RESET
*/	
void	error_exit(const char *error)
{
	printf(RED"🚨 %s 🚨\n"RST, error);
	exit(EXIT_FAILURE);
}
