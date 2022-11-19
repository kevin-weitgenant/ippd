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
      delete (frameR);
      delete (frameA);
    }
  }
  MPI_Finalize();
}

// mpic++ openMP_MAISframes.cpp -o teste
// mpirun --host localhost:4 teste