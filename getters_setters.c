#include "philo.h"

/*
 * 	SETTERS and GETTERS are very useful to avoid writing
 * 		lots of repetitive code, namely lock🔒 - unlock🔓
 *
 * 💡 I decided to use only bools and longs in my structs 
 * 		to use a limited nbr of setters & getters 💡
 *
 *  1) if you value type safety and performance and have a 
 *  		imited number of types, then this approach is excellent.
 *	2) If you value scalability and flexibility, writing all
 *			these setters and getters is not the goto solution 
*/

/*
 * Set a bool to value thread safe
*/
void	set_bool(t_mtx	*mutex, bool *dest, bool value)
{
	safe_mutex_handle(mutex, LOCK);
	*dest = value;
	safe_mutex_handle(mutex, UNLOCK);
}

bool	get_bool(t_mtx *mutex, bool *value)
{
	bool	ret;

	safe_mutex_handle(mutex, LOCK);
	ret = *value;
	safe_mutex_handle(mutex, UNLOCK);
	return (ret);
}

long	get_long(t_mtx *mutex, long *value)
{
	long	ret;

	safe_mutex_handle(mutex, LOCK);
	ret = *value;
	safe_mutex_handle(mutex, UNLOCK);
	return (ret);
}

void	set_long(t_mtx *mutex, long *dest, long value)
{
	safe_mutex_handle(mutex, LOCK);
	*dest = value;
	safe_mutex_handle(mutex, UNLOCK);
}

/*
 * I use simulation finished to make 
 * the code more readable, arguably this 
 * function is redundant being just a wrapper.
*/
bool	simulation_finished(t_table *table)
{
	return (get_bool(&table->table_mutex, &table->end_simulation));
}
