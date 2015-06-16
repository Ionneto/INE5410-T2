#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

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
        int nBuckets = atoi(argv[2]);

        if (tam_vetor < nBuckets) {
            printf("Algo errado não está certo");
        }

        int rank;
        int size;
        MPI_Status st;


///////////////////////////////////////////////////////////////////////
// Inicializa o ambiente MPI
///////////////////////////////////////////////////////////////////////

        MPI_Init(&argc, &argv);

///////////////////////////////////////////////////////////////////////
// PROCESSO MESTRE:
///////////////////////////////////////////////////////////////////////
        
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

         if (nBuckets > tam_vetor || size < 2) {
              printf("Por favor, use valores válidos de processos e tamanho de vetor");
              return 0;
          }

        if (rank == 0) {

        //////////////////////////////////////////////////////////////
        // Criar vetor aleatório e imprimir
        //////////////////////////////////////////////////////////////

            int i, j, aux;
            int *vetor = (int *) malloc(sizeof(int)*tam_vetor);

            srand((unsigned)time(NULL));

            for(i = 0; i < tam_vetor; i++){
                vetor[i] = rand() % tam_vetor;
                printf("%d ", vetor[i]);
            }

            printf("\n");

    //////////////////////////////////////////////////////////////
    // Criar vetor de buckets, criar buckets
    //////////////////////////////////////////////////////////////

        int resto = tam_vetor % nBuckets;
        int divisao = tam_vetor / nBuckets;
        s_bucket *listaDeBuckets = (s_bucket *) malloc(sizeof(s_bucket) * nBuckets);
        
        for(i = 0; i < nBuckets; i++) {
            listaDeBuckets[i].tam_bucket = 0;
            listaDeBuckets[i].minimo = 0;
            listaDeBuckets[i].maximo = 0;
        }

        if(resto != 0) {

            listaDeBuckets[0].minimo = 0;
            listaDeBuckets[0].maximo = divisao;

            for(i = 1; i < resto; i++) {
                listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
                listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + 1 + divisao;
            }

            for(i = resto; i < nBuckets; i++) {
                listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
                listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
            }

        } else {
            listaDeBuckets[0].minimo = 0;
            listaDeBuckets[0].maximo = divisao-1;

            for (i = 1; i < nBuckets; ++i) {
                listaDeBuckets[i].minimo = listaDeBuckets[i-1].maximo + 1;
                listaDeBuckets[i].maximo = listaDeBuckets[i-1].maximo + divisao;
            }
        }

            for(i = 0; i < nBuckets; i++) {
                for(j = 0; j < tam_vetor; j++) {
                    if(vetor[j] <= listaDeBuckets[i].maximo && vetor[j] >= listaDeBuckets[i].minimo)
                        listaDeBuckets[i].tam_bucket += 1;
                }
                                        
                listaDeBuckets[i].v = (int *) malloc(sizeof(int) * listaDeBuckets[i].tam_bucket);
            }

            for(i = 0; i < nBuckets; i++) {
                aux = 0;
                for(j = 0; j < tam_vetor; j++) {
                    if(vetor[j] <= listaDeBuckets[i].maximo && vetor[j] >= listaDeBuckets[i].minimo) {
                        listaDeBuckets[i].v[aux] = vetor[j];
                        aux++;
                    }
                }
            }

        for (i = 0; i < nBuckets; ++i) {
            printf("\nBucket %d: %d-%d\n", i, listaDeBuckets[i].minimo, listaDeBuckets[i].maximo);
            for(aux = 0; aux < listaDeBuckets[i].tam_bucket; aux++) {
                printf("%d, ", listaDeBuckets[i].v[aux]);
            }
        }





        //PROCESSOS
        
      int rankEscravo;
      int envios[3];
      envios[2] = 0;
      int *temp;

      for (i=0; i<nBuckets; i++) {
         if (listaDeBuckets[i].tam_bucket > 1) {
             MPI_Recv(&rankEscravo, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &st);
              printf("Enviando balde %d para escravo %d\n", i, rankEscravo);
              envios[0] = listaDeBuckets[i].tam_bucket;
              envios[1] = i;
              MPI_Send(&envios, 3, MPI_INT, rankEscravo, 2, MPI_COMM_WORLD);
              MPI_Send(listaDeBuckets[i].v, listaDeBuckets[i].tam_bucket, MPI_INT, rankEscravo, 3, MPI_COMM_WORLD);
              MPI_Recv(&envios, 3, MPI_INT, MPI_ANY_SOURCE, 4, MPI_COMM_WORLD, &st);
              printf("Recebendo balde ORDENADO %d do escravo %d \n", envios[1], envios[2]);
              
              temp = malloc(sizeof(int)*envios[0]);
              MPI_Recv(temp, envios[0], MPI_INT, MPI_ANY_SOURCE, 5, MPI_COMM_WORLD, &st);
              listaDeBuckets[envios[1]].tam_bucket = envios[0];
              listaDeBuckets[envios[1]].v = temp;
              }
      }
      envios[2] = -1;

       for(i=1; i<size; i++) {
          MPI_Send(&envios, 3, MPI_INT, i, 2, MPI_COMM_WORLD);
      }


      //REINSERÇÃO
      int cont = 0;
      for (i = 0; i<nBuckets; i++) {
          for (j = 0; j<listaDeBuckets[i].tam_bucket; j++) {
            vetor[cont] = listaDeBuckets[i].v[j];
            printf(" %d, ", vetor[cont]);
            cont++;
          }
      }

      
      free(vetor);
      free(listaDeBuckets);
      }
      //ESCRAVOS
      else {
          int envios[3];
          int *valuesBalde;
          envios[2] = 0;
          while(1){
            MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Recv(&envios, 3, MPI_INT, 0, 2, MPI_COMM_WORLD, &st);
              if (envios[2] == -1){
                  break;
              }
              valuesBalde = malloc(sizeof(int)*envios[0]);

              MPI_Recv(valuesBalde, envios[0], MPI_INT, 0, 3, MPI_COMM_WORLD, &st);
              bubble_sort(valuesBalde, envios[0]);

              envios[2] = rank;
              MPI_Send(&envios, 3, MPI_INT, 0, 4, MPI_COMM_WORLD);
              MPI_Send(valuesBalde, envios[0], MPI_INT, 0, 5, MPI_COMM_WORLD);

      }
  }
  MPI_Finalize();
  return 0;
}

