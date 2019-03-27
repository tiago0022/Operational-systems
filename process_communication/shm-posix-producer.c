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


int main()
{
	const int SIZE = 1000004096;
	const int VEC_SIZE = 500;
	const char *name = "shared_memory";

	int shm_fd;
	char *ptr, *start;

srand(time(NULL));   // Initialization, should only be called once.
int r = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.

	/* Cria um segmento de memória compartilhado
	e retorna um descritor de arquivo. Este
	arquivo não existe fisicamente em disco,
	ele é apenas um arquivo virtual criado em
	em um sistema de arquivos temporário (tmpfs)
	dentro de /dev/shm/xxx. */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/*shm_open: cria ou abre um objeto de
	memória compartilhado.

	/* O tamanho inicial de um segmento é de 0 bytes.
	A função "ftruncate" define o tamanho do
	segmento.*/
	ftruncate(shm_fd,SIZE);

	/* Mapeia o segmento de memória para o espaço
	 de endereçamento do processo. Dessa forma, o
	segmento poderá ser acessado por meio de um
	ponteiro.

	/* O primeiro parâmetro "0" indica que o kernel
	escolhe o endereço no qual o mapeamento será
	criado. Valores diferentes de "0" são utilizados
	como sugestões para o Kernel.
	O último parâmetro "0" indica o ponto de início
	do mapeamento no arquivo.
	No caso abaixo, o mapeamento contém SIZE bytes
	iniciando em 0.*/
	ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (ptr == MAP_FAILED) {
		printf("Map failed\n");
		return -1;
	}
	/*Depois que o segmento de memória foi
	mapeado para o espaço de endereçamento
	do processo, o arquivo ("shm_fd") pode
	ser fechado.*/

	/*Escreve na memória compartilhada.*/
	start = ptr;
	//sprintf(ptr,"Hello: ");
	//ptr += 7; //Move o ponteiro
	char c;
	int i;
	for (i=0; i < VEC_SIZE; ptr++, i++){
					c = rand()%26+65;
	      	*ptr = c;
	}
	*ptr = '\0';
	ptr++;
	*ptr='G';
	ptr++;
	*ptr = -1;
	// printf("aa %d\n", '\0');
	// ptr++;
	// *ptr = VEC_SIZE;
	// sleep(30);
	//memcpy(ptr,"conteudo",tamanho);
	return 0;
}
