#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>


#define NUMBER 5
#define MAX_SLEEP_TIME	3
enum {THINKING, HUNGRY, EATING} state[NUMBER];

// THREADS FOR PHILOSOPHERS
int thread_id[NUMBER];


pthread_cond_t		cond_vars[NUMBER];
pthread_mutex_t 	mutex_lock;

void *philosopher(void *param);
pthread_t tid[NUMBER];

void init()
{
	int i;

	for (i = 0; i < NUMBER; i++)
	{
		state[i] = THINKING;
		thread_id[i] = i;
		pthread_cond_init(&cond_vars[i],NULL);
	}

	pthread_mutex_init(&mutex_lock, NULL);


	for (i = 0; i < NUMBER; i++)
		pthread_create(&tid[i], 0, philosopher, (void *)&thread_id[i]);
}

void test(int i)
{
	if ( (state[(i + NUMBER - 1) % NUMBER] != EATING) && (state[i] == HUNGRY) && (state[(i + 1) % NUMBER] != EATING) )
	{
		state[i] = EATING;
		printf("Philosopher %d is eating\n", i);
		pthread_cond_signal(&cond_vars[i]);
	}
}

void pickup_forks(int number)
{
	pthread_mutex_lock(&mutex_lock);

	state[number] = HUNGRY;

	test(number);
	while (state[number] != EATING)
		pthread_cond_wait(&cond_vars[number], &mutex_lock);

	pthread_mutex_unlock(&mutex_lock);
}

void return_forks(int number)
{
	pthread_mutex_lock(&mutex_lock);

	state[number] = THINKING;
	printf("Philosopher %d is thinking\n", number);


	test((number + NUMBER - 1) % NUMBER);
	test((number + 1) % NUMBER);

	pthread_mutex_unlock(&mutex_lock);
}

void *philosopher(void *param)
{
	int number = *(int *)param;
	int sleep_time = (rand() % 3) + 1;



	while(1)
	{
		sleep(sleep_time);
		pickup_forks(number);

		sleep(sleep_time);
		return_forks(number);
	}
}

int main()
{
	init();

	int i;
	for (i = 0; i < NUMBER; i++)
		pthread_join(tid[i],NULL);
	return 0;
}
