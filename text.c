#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "synclib.h"
#include <string.h>
#define MAX_THREADS 4
myrwlock_t rw;
FILE *input, *output;
char text[] = "Maecenas et purus in risus consectetur vestibulum at sit amet leo. Praesent lacinia nunc vitae sagittis interdum. Vivamus molestie, elit bibendum iaculis posuere, odio ligula rutrum ligula, consequat dictum urna elit a diam. Nam cursus luctus mauris eu rhoncus. Mauris non quam at magna tempus iaculis quis vel tortor. Aenean mattis malesuada dui sit amet lobortis. Etiam quis tempus massa. In ut viverra ligula. Nam porta erat vel mauris tristique, gravida sodales quam ullamcorper. Quisque a ex ut augue faucibus efficitur.";

void* Read(void* argument)
{
	int* index = (int *) argument;
	input = fopen("text.txt", "r");
	char* temp = malloc(sizeof(char) * 20);
	myrwlock_readbegin(&rw);
	while(fscanf(input, "%s", temp) > 0)
	{
		myrwlock_readend(&rw);
		printf("Reader %d sees %s\n", (*index), temp);
		myrwlock_readbegin(&rw);
	}
	myrwlock_readend(&rw);
}	

void* Write(void* argument)
{
	int* index = (int *) argument;
	output = fopen("text.txt", "a");
	char* temp = strtok(text, " ");
	while(temp != NULL)
	{
		myrwlock_writebegin(&rw);
		printf("Writer %d writes %s\n", (*index), temp); 
		fprintf(output, "%s ", temp);
		myrwlock_writeend(&rw);
		temp = strtok(NULL, " ");
	}	
}		


int main()
{
	int i = 0;
	int *current;
	myrwlock_init(&rw);
	pthread_t thread[MAX_THREADS];
	while (i < MAX_THREADS)
	{
		current = malloc(sizeof(int));
		*current = i;
		if (i < MAX_THREADS/2)
			pthread_create(&thread[i], NULL,  Read, current);
		else pthread_create(&thread[i], NULL,  Write, current);
		i++;
	}
	i = 0;
	while (i < MAX_THREADS)
	{
		pthread_join(thread[i], NULL);
		i++;
	}
	myrwlock_destroy(&rw);
	fclose(input);
	fclose(output);
	return 0;
}


	
