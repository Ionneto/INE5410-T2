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

///////////////////////////////////////////////////////////////////////
// Inicializa o ambiente MPI
///////////////////////////////////////////////////////////////////////

        MPI_Init(&argc, &argv);

///////////////////////////////////////////////////////////////////////
// PROCESSO MESTRE:
///////////////////////////////////////////////////////////////////////
        
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);

        if (rank == 0) {

        //////////////////////////////////////////////////////////////
        // Criar vetor aleatório e imprimir
        //////////////////////////////////////////////////////////////

            int i, j, aux;
            int *vetor = (int *) malloc(sizeof(int)*tam_vetor);
            int *vetor_final = (int *) malloc(sizeof(int)*tam_vetor);

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


            for (i = 0; i < nBuckets; i++) {

                //////////////////////////////////////////////////////////////
                // Recebe msg com rank do processo que está livre
                //////////////////////////////////////////////////////////////
                
                int rankEscravo;
                printf("mestre aguardando o rank do escravo livre\n");
                fflush(stdout);

                MPI_Status st;
                 MPI_Recv(&rankEscravo, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &st);
                printf("mestre recebeu o rank do escravo livre %d\n", rankEscravo);
                fflush(stdout);
                


                if (listaDeBuckets[i].tam_bucket > 1) {

                //////////////////////////////////////////////////////////////
                // Envia buckets para os escravos
                //////////////////////////////////////////////////////////////
                
                    int buf[2];
                    buf[0] = listaDeBuckets[i].tam_bucket;
                    buf[1] = i;

                     MPI_Send(&buf, 2, MPI_INT, rankEscravo,    5, MPI_COMM_WORLD);
                    MPI_Send(listaDeBuckets[i].v, buf[0], MPI_INT, rankEscravo, 0, MPI_COMM_WORLD);    
                            
                    printf("Mestre enviou TAMANHO %d\n", buf[0]);
                    fflush(stdout);

                    printf("Mestre ENVIOU bucket %d para escravo %d\n", i, rankEscravo);
                    fflush(stdout);
 
                //////////////////////////////////////////////////////////////
                // Recebe os buckets organizados
                //////////////////////////////////////////////////////////////
                    
                    int *vetor_bucket;
                    MPI_Recv(&vetor_bucket, buf[0], MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    listaDeBuckets[i].v = vetor_bucket;

                }

            }

        //////////////////////////////////////////////////////////////
        // Reinserir os valores dos buckets no array principal
        //////////////////////////////////////////////////////////////

            aux = 0;

            for (i = 0; i < nBuckets; i++) {
                for (j = 0; j < listaDeBuckets[i].tam_bucket; j++){
                    vetor_final[aux] = listaDeBuckets[i].v[j];
                    aux++;
                }
            }

            // Imprime o vetor ordenado
            printf("\n \n");    
            for(i = 0; i < tam_vetor; i++){
                printf("%d ", vetor_final[i]);
            }
            printf("\n \n");


///////////////////////////////////////////////////////////////////////
// Processo escravo
///////////////////////////////////////////////////////////////////////

        } else {

            int *buf;
            int *vet_bucket;
            int info_bucket[2];

            while(1) {
                
                printf("tentando enviar o rank do escravo livre %d\n", rank);
                //envia msg com o seu numero de rank, pra avisar q está livre:
                 MPI_Send(&rank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                printf("enviou o rank do escravo livre %d\n", rank);
                fflush(stdout);
                
                 MPI_Recv(&info_bucket, 2, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //info_bucket[0] == tamanho do bucket
                //info_bucket[1] == id do bucket
                printf("escravo recebeu TAMANHO%d\n", info_bucket[0]);
                fflush(stdout);
                
                vet_bucket = (int*) malloc(sizeof(int)* info_bucket[0]);
                //recebe o vetor de inteiros do bucket:
                MPI_Recv(&vet_bucket, info_bucket[0], MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                printf("escravo recebeu O VETOR\n");
                fflush(stdout);

                printf("tentando ENVIAR O VETOR PRO BUBBLE\n");
                fflush(stdout);
                //envia o para o bubble_sort o vetor, e o tamanho dele
                bubble_sort(vet_bucket, info_bucket[0]);
                printf("escravo enviou o vetor pro bubble\n");
                fflush(stdout);

                //envia de volta para o mestre, o vetor ordenado pelo bubble_sort
                MPI_Send(&vet_bucket, info_bucket[0], MPI_INT, 0, 0, MPI_COMM_WORLD);
                printf("Mestre RECEBEU bucket %d de escravo %d\n", info_bucket[1], rank);    
            }            
        }



///////////////////////////////////////////////////////////////////////
// Finaliza o ambiente MPI
///////////////////////////////////////////////////////////////////////
MPI_Request *request;
MPI_Status *status;
    MPI_Wait(request, status);
    MPI_Finalize();

///////////////////////////////////////////////////////////////////////
// Libera os espaços alocados
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Fim do main
///////////////////////////////////////////////////////////////////////
    }


