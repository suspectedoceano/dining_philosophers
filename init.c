#include "philo.h"

/*
 * Every philo is assigned a first & second fork
 * 💡Why first and second? Not left and right?💡
 * 	This is the odd even method to avoid a deadlock.
 * 	It makes more sense, and the code cleaner when the 
 * 	simulation starts, to have a first-second fork 
 * 	for all the philosophers.
 *
 * 	Using i from the caller loop as a position value,
 * 	given that the philo ID is i + 1 this makes,
 * 	the code more readable
*/
static void	assign_forks(t_philo *philo, t_fork *forks,
		int philo_position)
{
	int	philo_nbr;

	philo_nbr = philo->table->philo_nbr;
	philo->first_fork = &forks[(philo_position + 1) % philo_nbr];
	philo->second_fork = &forks[philo_position];
	if (philo->id % 2 == 0)
	{
		philo->first_fork = &forks[philo_position];
		philo->second_fork = &forks[(philo_position + 1) % philo_nbr];
	}	
}

/*
 * Init all the necessary data for philosophers
*/
static void	philo_init(t_table *table)
{
	int		i;
	t_philo	*philo;

	i = -1;
	while (++i < table->philo_nbr)
	{
		philo = table->philos + i;
		philo->id = i + 1;
		philo->full = false;
		philo->meals_counter = 0;
		safe_mutex_handle(&philo->philo_mutex, INIT);
		philo->table = table;
		assign_forks(philo, table->forks, i);
	}
}

/*
 * Code to init the table data
 * Controls on errors embedded in 
 * safe functions
 *
 * Every fork gets an ID value 
 * useful for debugging:
 * 	You can see these values doing
 * 	~make DEBUG_MODE=1
*/
void	data_init(t_table *table)
{
	int		i;

	i = -1;
	table->end_simulation = false;
	table->all_threads_ready = false;
	table->threads_running_nbr = 0;
	table->philos = safe_malloc(table->philo_nbr * sizeof(t_philo));
	table->forks = safe_malloc(table->philo_nbr * sizeof(t_fork));
	safe_mutex_handle(&table->write_mutex, INIT);
	safe_mutex_handle(&table->table_mutex, INIT);
	while (++i < table->philo_nbr)
	{
		safe_mutex_handle(&table->forks[i].fork, INIT);
		table->forks[i].fork_id = i;
	}
	philo_init(table);
}
