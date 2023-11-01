#include "philo.h"

/*
 * Wait for all threads to be 
 * ready, busy waiting (spinlock)
 *
 * I use a getter function to read with no race 
 * condition the variable
*/
void	wait_all_threads(t_table *table)
{
	while (!get_bool(&table->table_mutex, &table->all_threads_ready))
		;
}

/*
 * Simple function to synchro monitoring thread and philos
	 * Monitor thread can start only when all threads are ready
 * When a philo enters the loop, increases the threads count
*/
void	increase_long(t_mtx *mutex, long *value)
{	
	safe_mutex_handle(mutex, LOCK);
	(*value)++;
	safe_mutex_handle(mutex, UNLOCK);
}

/*
 * Monitor waits all threads are running the 
 * simulation before searching deaths
*/
bool	all_threads_running(t_mtx *mutex, long *threads,
		long philo_nbr)
{
	bool	ret;

	ret = false;
	safe_mutex_handle(mutex, LOCK);
	if (*threads == philo_nbr)
		ret = true;
	safe_mutex_handle(mutex, UNLOCK);
	return (ret);
}

/*
 * Synchronize the philos to minimize
 * resource contention and 
 * improve fairness
 * 1) if even, just 30ms (half the min value 60ms)
 * 2) if odd, start by thinking
*/
void	de_synchronize_philos(t_philo *philo)
{
	if (philo->table->philo_nbr % 2 == 0)
	{
		if (philo->id % 2 == 0)
			precise_usleep(3e4, philo->table);
	}
	else
	{
		if (philo->id % 2)
			thinking(philo, true);
	}
}	
