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
  
  //start MPI
  int qtd_ranks, rank;
  MPI_Status st;
  char computador[MPI_MAX_PROCESSOR_NAME];
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &qtd_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf("Estou executando no computador %s, meu rank %d de um total de %d processos\n", computador, rank, qtd_ranks);
	//start MPI

  char *frameR;
  int buffer_len = 150;
  char frame[char_por_frame];
  
  if (rank == 0) {
    video = fopen("video_converted_640x360.yuv", "rb"); 
      
    for (int i = 1; i < qtd_ranks; i++) {
      frameR = readFrames(widthFrame, heightFrame, video); 
      
      //MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
      MPI_Send(frameR, char_por_frame, MPI_CHAR, i, 5, MPI_COMM_WORLD);
      printf("enviado frame\n");
      delete(frameR);

    }
  }

  else {
    frameR = new char[char_por_frame];
    
    MPI_Recv(frameR, char_por_frame, MPI_CHAR, 0, 5, MPI_COMM_WORLD, &st);
    //int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
    printf("recebido frame\n");
    delete(frameR);
  }
  MPI_Finalize();
}




//mpic++ openMP_lerFrames.cpp -o teste
//mpirun --host localhost:4 teste