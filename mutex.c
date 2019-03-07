#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "synclib.h"
#include <sys/types.h>
#include <unistd.h>
#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;
mymtx_t mtx;

int decrease_count(int count)
{
	if (available_resources < count)
		return -1;
	else
	{
		mymtx_lock(&mtx);
		available_resources -= count;
		printf("Got %d resources, %d remaining\n", count, available_resources);
		mymtx_unlock(&mtx);
	}
	return 0;
}

int increase_count(int count)
{
	mymtx_lock(&mtx);
	available_resources += count;
	printf("Released %d resources, %d remaining\n", count, available_resources);
	mymtx_unlock(&mtx);
	return 0;
}
	
void* pthread_func(void* substitute)
{
	mymtx_lock(&mtx);
        int count = rand()%5 + 1;
	mymtx_unlock(&mtx);
        decrease_count(count);
        increase_count(count);
}

int main()
{
	mymtx_init(&mtx);
	int i = 0;
	int random = 0;
	time_t t;
	pthread_t thread[5];
	srand((unsigned) time(&t));
	while (i < 5)
	{
		pthread_create(&thread[i], NULL,  pthread_func, NULL);
		i++;
	}
	i = 0;
	while (i  < 5)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
 	mymtx_destroy(&mtx);
	return 0;
}
