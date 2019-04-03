/**
 * Simple program demonstrating shared memory in POSIX systems.
 *
 * This is the consumer process
 *
 * Figure 3.18
 *
 * @author Gagne, Galvin, Silberschatz
 * Operating System Concepts - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

int getVectorInit(int vec_size, int myProcessNumber, int numberOfProcesses)
{
	int part = vec_size / numberOfProcesses * (myProcessNumber - 1);
	printf("myPartStart -> %d\n", part);
	return part;
}

void removeMem(char *name1st, char *name2nd)
{
	if (shm_unlink(name1st) == -1)
	{
		printf("Error removing %s\n", name1st);
		exit(-1);
	}

	if (shm_unlink(name2nd) == -1)
	{
		printf("Error removing %s\n", name2nd);
		exit(-1);
	}
}

int main()
{
	const char *vector_memory = "shared_memory";
	const char *sum_memory = "sum_memory";
	const int VEC_MEM_SIZE = 1000004096;
	const int SUM_MEM_SIZE = 1024;
	int numberOfProcesses = 0, myProcessNumber = 1;

	int vector_shm_fd, sum_shm_fd, i;
	int *vectorPtr, *vectorPtrStart;
	int *sumPtr, *sumPtrStart;

	/* open the shared memory segment */
	vector_shm_fd = shm_open(vector_memory, O_RDONLY, 0666);
	sum_shm_fd = shm_open(sum_memory, O_RDWR, 0666);
	if (vector_shm_fd == -1 || sum_shm_fd == -1)
	{
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the address space of the process */
	vectorPtr = (int *)mmap(0, VEC_MEM_SIZE, PROT_READ , MAP_SHARED, vector_shm_fd, 0);
	sumPtr = (int *)mmap(0, SUM_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sum_shm_fd, 0);
	if (vectorPtr == MAP_FAILED || sumPtr == MAP_FAILED)
	{
		printf("Map failed\n");
		exit(-1);
	}

	sumPtrStart = sumPtr;
	printf("Reading Sum Memory:\n");
	while (1)
	{
		int value = *sumPtr;
		printf("%d ", value);
		printf("%d  |  ", (int *)sumPtr);
		if (value == -2)
		{
			break;
		}
		if (value != -1)
		{
			myProcessNumber++;
		}
		numberOfProcesses++;
		sumPtr++;
	}
	printf("\nnumberOfProcesses = %d\n", numberOfProcesses);
	printf("myProcessNumber = %d\n", myProcessNumber);

	/* now read from the shared memory region */

	//
	char target = *((char *)(vectorPtr));
	int vec_size = (int)((strlen((char *)vectorPtr)) - 2);

	int myPartStart = getVectorInit(vec_size, myProcessNumber, numberOfProcesses);
	vectorPtrStart = vectorPtr;// + myPartStart;
	printf("%d / %d\n", vectorPtrStart, (vec_size / numberOfProcesses) + vectorPtrStart);
	// for (i = vectorPtrStart; i < ((vec_size / numberOfProcesses) + vectorPtrStart); i++)
	int count = 0;
	for (i = 2; i < (int)((vec_size / numberOfProcesses) +2); i++)
	{
		// printf("posicao %d, valor %c  |  ", i+myPartStart, ((char *)vectorPtrStart)[i+myPartStart]);
		if (((char *)vectorPtrStart)[i+myPartStart] == target)
			count++;
	}

	printf("\n\nReading Vector Memory:\n");
	printf("%d letras %c\n", count, target);
	// printf("vetor -> %s\n", (char *)(vectorPtr));
	printf("vec_size -> %d\n", vec_size);
	printf("numberOfProcess -> %d\n", numberOfProcesses);

	//Save count on sum_vector
	printf("\n%d + %d\n", (int *)sumPtrStart, myProcessNumber - 1);
	sumPtr = (sumPtrStart + (myProcessNumber - 1));
	printf("sumPtr -> %d\n", (int *)sumPtr);
	*sumPtr = count;
	printf("%d\n", *sumPtr);

	printf("\nReading Sum Memory refreshed:\n");
	sumPtr = sumPtrStart;
	while (1)
	{
		int value = *sumPtr;
		printf("%d ", value);
		if (value == -2)
		{
			break;
		}
		if (value != -1)
		{
			myProcessNumber++;
		}
		numberOfProcesses++;
		sumPtr++;
	}
	puts("");

	/* remove the shared memory segment */
	//removeMem(vector_memory, sum_memory);

	return 0;
}
