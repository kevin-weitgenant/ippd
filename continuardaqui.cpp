#include <iostream>
using namespace std;
#include "mpi.h"
#include <cstdlib>
#include <time.h>
#include <vector>

int const numeroFrames = 120;
int const heightFrame = 360;
int const widthFrame = 640;
int const char_por_frame = heightFrame * widthFrame;
FILE *video;

char *readFrames(int width, int heigth, FILE *video) {
  char *frameY, *Lixo;

  frameY = (char *)malloc((size_t)(width * heigth));
  Lixo = (char *)malloc((size_t)(width * heigth / 2)); // 2*(x/2,y/2) =  (x,y/2)

  fread(frameY, (size_t)(width), (size_t)(heigth), video);
  fread(Lixo, (size_t)(width), (size_t)(heigth / 2), video);
  free(Lixo);

  return frameY;
}

int main(int argc, char *argv[]) {

  // start MPI
  int qtd_ranks, rank;
  MPI_Status st;
  char computador[MPI_MAX_PROCESSOR_NAME];
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &qtd_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Estou executando no computador %s, meu rank %d de um total de %d processos\n", computador, rank, qtd_ranks);
  // start MPI

  char *frameR, *frameA;

  if (rank == 0) {
    video = fopen("video_converted_640x360.yuv", "rb");
    frameR = readFrames(widthFrame, heightFrame, video);

    for (int iFrame = 0; iFrame < numeroFrames - 1; iFrame += qtd_ranks - 1) {
      for (int rank_destino = 1; rank_destino < qtd_ranks; rank_destino++) {
        frameA = readFrames(widthFrame, heightFrame, video);

        // MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
        MPI_Send(frameR, char_por_frame, MPI_CHAR, rank_destino, 0, MPI_COMM_WORLD);
        printf("Enviado requisição de frameR %d", rank_destino);
        MPI_Send(frameA, char_por_frame, MPI_CHAR, rank_destino, 1, MPI_COMM_WORLD);
        printf("Enviado requisição de frameA %d", rank_destino);
        delete (frameR);
        frameR = frameA;
      }
    }
    delete (frameR);
  }

  else {
    

    for (int iFrame = 0; iFrame < numeroFrames - 1; iFrame += qtd_ranks - 1) {
      frameR = new char[char_por_frame];
      frameA = new char[char_por_frame];
      
      // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
      MPI_Recv(frameR, char_por_frame, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &st);
      printf("recebido frame R");
      MPI_Recv(frameA, char_por_frame, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &st);
      printf("recebido frame A");
      
      for ( int h = 0; h <= heightFrame- sizeBlock; h+=sizeBlock){//dividir frame A em blocos sem superposição
            for ( int w = 0; w <= widthFrame- sizeBlock; w+=sizeBlock){      
                
              	
                block = getblock(frameA, w, h, sizeBlock); //pega um bloco sem sobreposição em A
                

                vetor Rv = findBestBlock(block, frameR,sizeBlock,widthFrame, heightFrame); //retorna o vetor do melhor bloco no frame de referencia
                deleteMatrix(block,sizeBlock,sizeBlock);
               
                //printf("Ra(%d,%d),Rv(%d,%d)\n",h,w,Rv.H,Rv.W);
                
              	index = ( h* ( (widthFrame - sizeBlock) / sizeBlock ) + w )*4;
              	vetorResultado[index] = h;				//0 4 8
                vetorResultado[1+index] = w;			//1 5 9
                vetorResultado[2+index] = Rv.H;	//2 6 10
                vetorResultado[3+index] = Rv.W;	//3 7 11
            }
        }
      delete (frameR);
      delete (frameA);
      MPI_Send(vetorResultado, 4*num_exec_for_for, MPI_INT, 0, 100, MPI_COMM_WORLD);   

    }
  }
  MPI_Finalize();
}

// mpic++ openMP_MAISframes.cpp -o teste
// mpirun --host localhost:4 teste