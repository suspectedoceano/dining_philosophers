#include "philo.h"

/*
 * Refine control over thinking
 *
 * 1) Print the status and then check.
 * If the philo nbr is even, i don't care return.
 *
 * Else
 * 	i force to think philo 42% of the time
 * and the % remaining is handled by contention.
 * Unconventional solution, 42 is a "Magic number"
 * but in test cases  works.
 * The thing is that i want to avoid a philo
 * 	immediately eating after sleeping without 
 * 	waiting a little for neighbour philo. 
 * I tried many cases, but not sure if 100% robust.
*/
void	thinking(t_philo *philo, bool pre_simulation)
{
	long	t_eat;
	long	t_sleep;
	long	t_think;

	if (!pre_simulation)
		write_status(THINKING, philo, DEBUG_MODE);
	if (philo->table->philo_nbr % 2 == 0)
		return ;
	t_eat = philo->table->time_to_eat;
	t_sleep = philo->table->time_to_sleep;
	t_think = (t_eat * 2) - t_sleep;
	if (t_think < 0)
		t_think = 0;
	precise_usleep(t_think * 0.42, philo->table);
}

/*
 * LONE philo
 * 0) Same algo preliminary code as all the threads
 * 1) Faking to lock the first_fork with write_status
 * 		just for output purpose
 * 2) Monitor will eventually spot the philo death
 * 		turning on end_simulation
*/
void	*lone_philo(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	wait_all_threads(philo->table);
	set_long(&philo->philo_mutex, &philo->last_meal_time, gettime(MILLISECOND));
	increase_long(&philo->table->table_mutex,
		&philo->table->threads_running_nbr);
	write_status(TAKE_FIRST_FORK, philo, DEBUG_MODE);
	while (!simulation_finished(philo->table))
		precise_usleep(200, philo->table);
	return (NULL);
}

/*
 * Eating routine
 * 1) Grab forks: here first & second fork is handy,
 * 		philo does not care if left or right
 * 2) eat: write status & update meals_counter, last_meal_time, 
 * 		full bool. 
 * 3) release forks
 *
 * 💡 last_meal_time in my implementation
 * 			happens before Eating :
 * 		this is helpful to avoid deaths 
 * 		while a philo is eating 💡
 *
 * 🔒 All the variables update are thread safe
 * 	thanks to setters, 
 * 	cause monitor thread read these values 
 * 	concurrently 🔒
*/
static void	eat(t_philo *philo)
{
	safe_mutex_handle(&philo->first_fork->fork, LOCK);
	write_status(TAKE_FIRST_FORK, philo, DEBUG_MODE);
	safe_mutex_handle(&philo->second_fork->fork, LOCK);
	write_status(TAKE_SECOND_FORK, philo, DEBUG_MODE);
	set_long(&philo->philo_mutex, &philo->last_meal_time, gettime(MILLISECOND));
	philo->meals_counter++;
	write_status(EATING, philo, DEBUG_MODE);
	precise_usleep(philo->table->time_to_eat, philo->table);
	if (philo->table->nbr_limit_meals > 0
		&& philo->meals_counter == philo->table->nbr_limit_meals)
		set_bool(&philo->philo_mutex, &philo->full, true);
	safe_mutex_handle(&philo->first_fork->fork, UNLOCK);
	safe_mutex_handle(&philo->second_fork->fork, UNLOCK);
}

/*
 * Actual dinner
 * 1) Wait for all threads to be ready
 * 2) Increase the number of running threads, useful for monitor
 * 3) de_synchronize_philos-> Useful for fairness
 * 4) Start an endless loop, until a philo eventually dies
 * 		or becomes full. 
 * 💡  write_status will always check for end_simulation 
 *     flag before writing 💡
*/
static void	*dinner_simulation(void *data)
{
	t_philo		*philo;

	philo = (t_philo *)data;
	wait_all_threads(philo->table);
	set_long(&philo->philo_mutex, &philo->last_meal_time,
		gettime(MILLISECOND));
	increase_long(&philo->table->table_mutex,
		&philo->table->threads_running_nbr);
	de_synchronize_philos(philo);
	while (!simulation_finished(philo->table))
	{
		if (get_bool(&philo->philo_mutex, &philo->full))
			break ;
		eat(philo);
		write_status(SLEEPING, philo, DEBUG_MODE);
		precise_usleep(philo->table->time_to_sleep, philo->table);
		thinking(philo, false);
	}
	return (NULL);
}

/*
 * 0) If no meals, return to main and clean
 * 0.1) If only one philo, create ad hoc thread 
 * 1) Create all the philosophers
 * 2) Create another one thread searching 
 * 		for death ones
 * 3) set time_start_simulation and 
 * 		set true ready all, so they can start simulation
 * 4) Wait for all
 * 5) If we pass line 164 it means all philos are full
 * 		so set end_simulation for monitor
 * 6) Wait for monitor as well. Then jump to clean in main
 *
 * 💡 If we join all threads it means they are all full,
 * so the simulation is finished, therefore we set the 
 * end_simulation ON 💡
 *
 * Useful cause also monitor uses this boolean, can be
 * turned on 
 * 	~by philos ending cause full
 * 	~by monitor cause a philo died
 * 💡It's a "2 way" bool for threads communication 💡
*/
void	dinner_start(t_table *table)
{
	int			i;

	i = -1;
	if (0 == table->nbr_limit_meals)
		return ;
	else if (1 == table->philo_nbr)
		safe_thread_handle(&table->philos[0].thread_id, lone_philo,
			&table->philos[0], CREATE);
	else
		while (++i < table->philo_nbr)
			safe_thread_handle(&table->philos[i].thread_id, dinner_simulation,
				&table->philos[i], CREATE);
	safe_thread_handle(&table->monitor, monitor_dinner, table, CREATE);
	table->start_simulation = gettime(MILLISECOND);
	set_bool(&table->table_mutex, &table->all_threads_ready, true);
	i = -1;
	while (++i < table->philo_nbr)
		safe_thread_handle(&table->philos[i].thread_id, NULL, NULL, JOIN);
	set_bool(&table->table_mutex, &table->end_simulation, true);
	safe_thread_handle(&table->monitor, NULL, NULL, JOIN);
}
