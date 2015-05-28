#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


///////////////////////////////////////////////////////////////////////
// Struct & shenanigans
///////////////////////////////////////////////////////////////////////

typedef struct {
	int *v;
	int tam_bucket;
	int minimo;
	int maximo;
} s_bucket;

///////////////////////////////////////////////////////////////////////
// Bubble_sort
///////////////////////////////////////////////////////////////////////

void bubble_sort ( int *v , int tam ) {
	int i, j, temp, trocou;
	for (j = 0; j < tam - 1; j ++) {
		trocou = 0;
	for (i = 0; i < tam - 1; i ++) {
		if ( v [ i + 1] < v [ i ]) {
			temp = v [ i ];
			v [ i ] = v [ i + 1];
			v [ i + 1] = temp;
			trocou = 1;
		}
	}
	if (!trocou) 
		break;
	}
}

///////////////////////////////////////////////////////////////////////
// ~~~LE MAIN~~~
///////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

///////////////////////////////////////////////////////////////////////
// Pegar parametros de entrada
///////////////////////////////////////////////////////////////////////

  int tam_vetor = atoi(argv[1]);
  int nProcessos = atoi(argv[2]);
  int nBuckets = atoi(argv[3]);

///////////////////////////////////////////////////////////////////////
// Criar vetor aleatório e imprimir
///////////////////////////////////////////////////////////////////////

  	int i;
	int *vetor = (int *) malloc(sizeof(int)*tam_vetor);
	srand((unsigned)time(NULL));
	for(i = 0; i < tam_vetor; i++){
		vetor[i] = rand() % tam_vetor;
		printf("%d ", vetor[i]);
	}
	printf("\n");

///////////////////////////////////////////////////////////////////////
// Criar vetor de buckets, criar buckets
///////////////////////////////////////////////////////////////////////

	int resto = tam_vetor % nBuckets;
	int divisao = tam_vetor / nBuckets;
	s_bucket *listaDeBuckets = (s_bucket *) malloc(sizeof(s_bucket) * nBuckets);

	if(resto != 0) {
		printf("a\n");
		listaDeBuckets[0].minimo = 0;
		listaDeBuckets[0].maximo = divisao;
					printf("%d %d\n",listaDeBuckets[0].minimo, listaDeBuckets[0].maximo);

		for(i = 1; i < resto; i++) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + 1 + divisao;
			printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
			}
		listaDeBuckets[i].v = (int *) malloc(sizeof(int) * listaDeBuckets[i].tam_bucket);

		
		for(i = resto; i < nBuckets; i++) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
						printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);

		}
		listaDeBuckets[i].v = (int *) malloc(sizeof(int) * listaDeBuckets[i].tam_bucket);



	} else {
		listaDeBuckets[0].minimo = 0;
		listaDeBuckets[0].maximo = divisao-1;
									printf("%d %d\n",listaDeBuckets[0].minimo, listaDeBuckets[0].maximo);

		for (i = 1; i < nBuckets; ++i) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
									printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);

		}
		listaDeBuckets[i].v = (int *) malloc(sizeof(int) * listaDeBuckets[i].tam_bucket);

	}

///////////////////////////////////////////////////////////////////////
// MPI SHENANIGANS - LEARN HOW TO DO THIS
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Reinserir os valores dos buckets, ordenadamente, no array principal
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Libera os espaços alocados
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Fim do main
///////////////////////////////////////////////////////////////////////

}
