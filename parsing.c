#include "philo.h"
#include <stdio.h>

/*
 * 2 handy functions just to make 
 * the code more readable
*/
static inline bool	is_space(char c)
{
	return ((c >= 9 && c <= 13) || 32 == c);
}

static inline bool	is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

/*
 * 1) Check for negatives
 * 2) Check if the number is legit 
 * "      +77fd$  "✅
 * "      +%$24   "🚨
 * 3) I check also len "2147483648", if > 10 means bigger INT_MAX
 * this will be useful for 12093838384775929283947592283948586
 * these crazy inputs. In atol i will refine the check
 *
 * Return a pointer to the actual number for atol
 * 
*/
static const char	*valid_input(const char *str)
{
	int			len;
	const char	*number;

	len = 0;
	while (is_space(*str))
		++str;
	if (*str == '+')
		++str;
	else if (*str == '-')
		error_exit("Feed me only positive values");
	if (!is_digit(*str))
		error_exit("The input is not a correct digit");
	number = str;
	while (is_digit(*str++))
		++len;
	if (len > 10)
		error_exit("The value is too big, INT_MAX is the limit");
	return (number);
}

/*
 * Convert the input into a long,
 * embedded a valid_input function
 * to check input validity
 *
 * The check for INT_MAX here is 
 * for numbers in range:
 * (2_147_483_648 <-> 9_999_999_999)
*/
static long	ft_atol(const char *str)
{
	long	num;

	num = 0;
	str = valid_input(str);
	while (is_digit(*str))
		num = (num * 10) + (*str++ - '0');
	if (num > INT_MAX)
		error_exit("INT_MAX is the limit, not the sky");
	return (num);
}

/*
 * 🚨 times in milliseconds 🚨
 * for the usleep function we need micro
 * so i immediately convert
 *  i use handy scientific notation 
 *  ~1e3 = 1_000
 *  ~1e6 = 1_000_000
 *
 * INPUT 
 * [0] ./philo
 * [1] number_of_philosophers
 * [2] time_to_die
 * [3] time_to_eat
 * [4] time_to_sleep
 * [5] [number_of_times_each_philosopher_must_eat]
 *
 * Check for max 200 philos
 * and timestamps > 60ms
 *
 * nbr_limit_meals -1 acts as a flag:
 * 	NO LIMITS
*/
void	parse_input(t_table *table, char **av)
{
	table->philo_nbr = ft_atol(av[1]);
	if (table->philo_nbr > PHILO_MAX)
	{
		printf(RED"Max philos are %d\n"
			G"make fclean and re-make with PHILO_MAX=nbr to change it\n"RST,
			PHILO_MAX);
		exit(EXIT_FAILURE);
	}
	table->time_to_die = ft_atol(av[2]) * 1e3;
	table->time_to_eat = ft_atol(av[3]) * 1e3;
	table->time_to_sleep = ft_atol(av[4]) * 1e3;
	if (table->time_to_die < 6e4
		|| table->time_to_sleep < 6e4
		|| table->time_to_eat < 6e4)
		error_exit("Use timestamps major than 60ms");
	if (av[5])
		table->nbr_limit_meals = ft_atol(av[5]);
	else
		table->nbr_limit_meals = -1;
}
