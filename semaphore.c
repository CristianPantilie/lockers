#include <stdio.h>
#include <stdlib.h>
#include "synclib.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#define max_semaphore 7

mymtx_t mtx;
mysem_t sem;
int value = 0;

void barrier_point()
{
        mymtx_lock(&mtx);
        value++;
	mymtx_unlock(&mtx);
        if (value == max_semaphore)
                mysem_post(&sem);
        else
        {
		mysem_wait(&sem);
		mysem_post(&sem);
        }
}


void* tfun(void* v)
{
	int *tid = (int *)v;
	printf("%d reached the barrier\n", *tid);
	barrier_point();
	printf("%d passed the barrier \n", *tid);
	free(tid);
	return NULL;
}

int main()
{
	int i = 0;
	mymtx_init(&mtx);
	mysem_init(&sem, 0);
	pthread_t thrd[max_semaphore];
	int *current;
	while (i < max_semaphore)
	{
		current = malloc(sizeof(int));
		*current = i;
		pthread_create(&thrd[i], NULL, tfun, current);
		i++;
	}
	i = 0;
	while (i < max_semaphore)
	{
		pthread_join(thrd[i], NULL);
		i++;
	}
	mysem_destroy(&sem);
	return 0;
}
