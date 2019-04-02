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
	printf("part -> %d\n", part);
	return part;
}

int main()
{
	const char *vector_memory = "shared_memory";
	const char *sum_memory = "sum_memory";
	const int VEC_MEM_SIZE = 1000004096;
	const int SUM_MEM_SIZE = 1024;
	int numberOfProcesses = 0, myProcessNumber = 1;
	//const int SIZE = 1000004096;

	int vector_shm_fd, sum_shm_fd, i;
	char *vectorPtr, *start;
	int *sumPtr;

	/* open the shared memory segment */
	vector_shm_fd = shm_open(vector_memory, O_RDONLY, 0666);
	sum_shm_fd = shm_open(sum_memory, O_RDWR, 0666);
	if (vector_shm_fd == -1 || sum_shm_fd == -1)
	{
		printf("shared memory failed\n");
		exit(-1);
	}

	/* now map the shared memory segment in the
	address space of the process */
	vectorPtr = mmap(0, VEC_MEM_SIZE, PROT_READ, MAP_SHARED, vector_shm_fd, 0);
	sumPtr = (int *)mmap(0, SUM_MEM_SIZE, PROT_READ, MAP_SHARED, sum_shm_fd, 0);
	if (vectorPtr == MAP_FAILED || sumPtr == MAP_FAILED)
	{
		printf("Map failed\n");
		exit(-1);
	}

	printf("Reading Sum Memory:\n");

	while (1)
	{
		int value = *sumPtr;
		printf("%d ", value);
		if (value == '\0')
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
	int count = 0;

	puts("antes do putsssss");
	char target = *((char *)(vectorPtr));
	// char numberOfProcessChar = *((char *)(vectorPtr + 1));
	// int numberOfProcess = CharToInt(numberOfProcessChar);
	int vec_size = (int *)(strlen(vectorPtr) - 2);

	start = vectorPtr + getVectorInit(vec_size, myProcessNumber, numberOfProcesses);
	printf("%d / %d\n", start, (vec_size / numberOfProcesses) + start);
	for (i = start; i < (vec_size / numberOfProcesses) + start; i++)
	{
		//printf("%d ", i-start);
		if (*((char *)vectorPtr + i) == target)
			count++;
	}
		puts("putssssssss");
	printf("\n\nReading Vector Memory:\n");
	printf("%d letras %c\n", count, target);
	printf("vetor -> %s\n", (char *)(vectorPtr + myProcessNumber));
	printf("vec_size -> %d\n", vec_size);
	printf("numberOfProcess -> %d\n", numberOfProcesses);

	// for (i = 0; i < numberOfProcess; i++, ptr++)
	// {
	// 	*ptr = -1;
	// 	ptr += i;
	// }

	// sleep(10);
	/* remove the shared memory segment */
	// if (shm_unlink(name) == -1) {
	// 	printf("Error removing %s\n",name);
	// 	exit(-1);
	// }

	return 0;
}
