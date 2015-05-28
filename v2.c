#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


///////////////////////////////////////////////////////////////////////
// Struct & shenanigans
///////////////////////////////////////////////////////////////////////
// Struct s_bucket, contendo o vetor do bucket, seu tamanho e seus limites.
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
  	int j;
  	int aux;
	int *vetor = (int *) malloc(sizeof(int)*tam_vetor);
	int *vetor_final = (int *) malloc(sizeof(int)*tam_vetor);

	// Inicia a seed do "rand", preenche o vetor com números aleatórios e printa o vetor.
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

	// Se a divisão não for exata, segue o procedimento de fazer "resto" buckets com 1
	// elemento a mais que os demais.
	if(resto != 0) {
		listaDeBuckets[0].minimo = 0;
		listaDeBuckets[0].maximo = divisao;
		//printf("%d %d\n",listaDeBuckets[0].minimo, listaDeBuckets[0].maximo);

		// Define os ranges dos baldes até "resto-1"
		for(i = 1; i < resto; i++) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + 1 + divisao;
			//printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
		}
		
		// Define o range do baldes resto e além de resto.
		for(i = resto; i < nBuckets; i++) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
			//printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
		}


	// Caso a divisão seja exata, define todos os buckets com o mesmo range
	// de "n" até "n + divisão - 1", contendo "divisão elementos"
	} else {
		listaDeBuckets[0].minimo = 0;
		listaDeBuckets[0].maximo = divisao-1;
		//printf("%d %d\n",listaDeBuckets[0].minimo, listaDeBuckets[0].maximo);

		for (i = 1; i < nBuckets; ++i) {
			listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
			listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
			//printf("%d %d\n",listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
		}
	}


	// Para cada bucket, busca os elementos do vetor que estão nele e aumenta sem "tam_bucket"
	// então aloca, para cara bucket, o espaço do seu vetor.
	for(i = 0; i < nBuckets; i++) {
		for(j = 0; j < tam_vetor; j++) {
			if(vetor[j] <= listaDeBuckets[i].maximo && vetor[j] >= listaDeBuckets[i].minimo)
				listaDeBuckets[i].tam_bucket += 1;
		}
		listaDeBuckets[i].v = (int *) malloc(sizeof(int) * listaDeBuckets[i].tam_bucket);
	}
	
	// Para cada bucket, faz a mesma busca anterior, dessa vez inserindo esses inteiros no
	// vetor do bucket, se este estiver no seu range.
	for(i = 0; i < nBuckets; i++) {
		aux = 0;
		for(j = 0; j < tam_vetor; j++) {
			if(vetor[j] <= listaDeBuckets[i].maximo && vetor[j] >= listaDeBuckets[i].minimo) {
				listaDeBuckets[i].v[aux] = vetor[j];
				aux++;
			}
		}
	}

	// Print para teste, lindão <3
	for (i = 0; i < nBuckets; ++i) {			
			printf("\nBucket %d: %d-%d\n", i, listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
			for(aux = 0; aux < listaDeBuckets[i].tam_bucket; aux++) {
				printf("%d, ", listaDeBuckets[i].v[aux]);
			}
		}

///////////////////////////////////////////////////////////////////////
// MPI SHENANIGANS - LEARN HOW TO DO THIS
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Reinserir os valores dos buckets, ordenadamente, no array principal
///////////////////////////////////////////////////////////////////////

	// Insere os vetores dos buckets novamente a um único vetor.
	aux = 0;
	for (i = 0; i < nBuckets; i++) {
		for (j = 0; j < listaDeBuckets[i].tam_bucket; j++){
			vetor_final[aux] = listaDeBuckets[i].v[j];
			aux++;
		}
	}

	// Imprime o vetor ordenado
	for(i = 0; i < tam_vetor; i++){
		printf("%d ", vetor_final[i]);
	}
	printf("\n");


///////////////////////////////////////////////////////////////////////
// Libera os espaços alocados
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// Fim do main
///////////////////////////////////////////////////////////////////////

}
