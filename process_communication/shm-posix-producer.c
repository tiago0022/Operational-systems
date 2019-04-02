//gcc -o producer shm-posix-producer.c -lrt

/**
 * Simple program demonstrating shared memory in POSIX systems.
 *
 * This is the producer process that writes to the shared memory region.
 *
 * Figure 3.17
 *
 * @author Silberschatz, Galvin, and Gagne
 * Operating System Concepts  - Ninth Edition
 * Copyright John Wiley & Sons - 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <time.h>
#include <stdlib.h>

int CharToInt(char c)
{
	return 48 - c;
}

char GetRandChar()
{
	int r = rand(); // Returns a pseudo-random integer between 0 and RAND_MAX.
	return rand() % 26 + 65;
}
int main()
{
	const int VEC_MEM_SIZE = 1000004096;
	const int SUM_MEM_SIZE = 1024;
	const int VEC_SIZE = 500;
	const char NUMBER_OF_PROCESSES = 4;
	const char CHAR_SEARCHED = 'G';
	const char *vector_memory = "shared_memory";
	const char *sum_memory = "sum_memory";

	int vector_shm_fd, sum_shm_fd, i;
	char *vectorPtr, *start;
	int *sumPtr;

	srand(time(NULL)); // Initialization, should only be called once.

	/* Cria um segmento de memória compartilhado e retorna um descritor de arquivo. Este
	arquivo não existe fisicamente em disco, ele é apenas um arquivo virtual criado em
	em um sistema de arquivos temporário (tmpfs) dentro de /dev/shm/xxx. */
	vector_shm_fd = shm_open(vector_memory, O_CREAT | O_RDWR, 0666);
	sum_shm_fd = shm_open(sum_memory, O_CREAT | O_RDWR, 0666);

	/* O tamanho inicial de um segmento é de 0 bytes. A função "ftruncate" define o tamanho do segmento.*/
	ftruncate(vector_shm_fd, VEC_MEM_SIZE);
	ftruncate(sum_shm_fd, SUM_MEM_SIZE);

	/* O primeiro parâmetro "0" indica que o kernel escolhe o endereço no qual o mapeamento será
	criado. Valores diferentes de "0" são utilizados como sugestões para o Kernel.
	O último parâmetro "0" indica o ponto de início do mapeamento no arquivo.
	No caso abaixo, o mapeamento contém SIZE bytes iniciando em 0.*/
	vectorPtr = mmap(0, VEC_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, vector_shm_fd, 0);
	sumPtr = mmap(0, SUM_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sum_shm_fd, 0);
	if (vectorPtr == MAP_FAILED || sumPtr == MAP_FAILED)
	{
		printf("Map failed\n");
		return -1;
	}

	/*Escreve na memória compartilhada.*/
	start = vectorPtr;
	//vectorPtr += 7; //Move o ponteiro
	*vectorPtr = CHAR_SEARCHED;
	vectorPtr++;
	*vectorPtr = '4';
	vectorPtr++;

	printf("Writing in Sum Memory:\n");
	for (i = 0; i < (int) NUMBER_OF_PROCESSES; i++)
	{
		*sumPtr = -1;
		printf("%d ", *(sumPtr));
		sumPtr++;
	}
	*sumPtr = '\0';
	printf("\n\n");

	char c;
	//int i;
	printf("Writing in Vector Memory:\n");
	printf("vetor -> ");
	for (i = 0; i < VEC_SIZE; vectorPtr++, i++)
	{
		c = GetRandChar();
		printf("%c", c);
		*vectorPtr = c;
	}
	printf("\n");
	*vectorPtr = '\0';
	vectorPtr++;
	for (i = 0; i < NUMBER_OF_PROCESSES; i++, vectorPtr++)
	{
		*vectorPtr = -1;
		vectorPtr += i;
	}
	// *vectorPtr = 'G';
	// vectorPtr++;
	// *vectorPtr = NUMBER_OF_PROCESSES;
	// vectorPtr++;

	//memcpy(vectorPtr,"conteudo",tamanho);
	return 0;
}
