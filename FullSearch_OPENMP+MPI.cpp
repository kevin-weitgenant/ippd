#include <iostream>
using namespace std;
#include "mpi.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <fstream>

typedef struct 
{
    int W;     
    int H; 
} vetor; 

typedef struct 
{  
    int SAD;    
    vetor posicao;
} blocoCandidato; 

int const numeroFrames = 20;
int const heightFrame = 360;
int const widthFrame = 640;
int sizeBlock = 8;
FILE *video;

//para logica MPI
int const char_por_frame = heightFrame * widthFrame;
//int num_exec_for_for = ((widthFrame - sizeBlock) / sizeBlock) * ((heightFrame - sizeBlock) / sizeBlock);
int num_exec_for_for = 3600;

//para logica MPI

char acessFrameArrayAsMatrix(char *vetor, int h, int w){
    return vetor[widthFrame*h + w];
}

void deleteMatrix(char **matrix,int width,int heigth){

    for( int h = 0 ; h < heigth ; h++ )
    {
        delete[] matrix[h]; 
    }
    delete[] matrix;
}

char * readFrames(int width,int heigth, FILE *video){
    char *frameY, *Lixo;

    frameY = (char*) malloc((size_t) (width * heigth));
    Lixo = (char*) malloc((size_t) (width * heigth/2));   // 2*(x/2,y/2) =  (x,y/2) 

    
    fread(frameY, (size_t) (width), (size_t) (heigth), video);
    fread(Lixo, (size_t) (width), (size_t) (heigth/2), video);
      
    return frameY;
}

/*void writeFrame(int width, int height, char *frameY, char *fileName){
    FILE *fileFrameY = fopen(fileName, "wb");//Input file
    fwrite(frameY, (size_t) (width * height), 1, fileFrameY);
    fclose(fileFrameY);
}*/


char ** getblock(char * frame,int pixelX,int pixelY,int size){
    int hBlock =0;
    int wBlock = 0;
    char** block = 0;
    block = new char*[size];
    
    for (int h = pixelY; h < pixelY+size; h++){
        block[hBlock] = new char[size];
        wBlock = 0;
        for (int w = pixelX; w < pixelX+size; w++){
            block[hBlock][wBlock]= acessFrameArrayAsMatrix(frame,h,w);
            wBlock++;
        }
        hBlock++;          
    }

    return block;
}
	
int SAD(int sizeblock,char** block1,char **block2){
    int result =0;

    for (int h = 0; h < sizeblock; h++){    
        for (int w = 0; w < sizeblock; w++){
            result += abs(block1[h][w]- block2[h][w]);
        }
    }
    return result;
} 

vetor findBestBlock(char **blocoAtual, char *frameR,int sizeBlock, int widthFrame,int heightFrame){  
                                                                        //retorna a posição do bloco dentro do frame(MELHORAR PRA JANELA SE DER TEMPO)
                                                                        //que tem o menor SAD
    int h = 0;                                                                              
    int w = 0;                                                                           
    int blocoCount = 0;
    char **block;
    vector<blocoCandidato> blocosCandidatos;
    int count = 0;

    // esses 2 FOR, adicionam no vetor blocosCandidatos, vários blocos armazenando a posição e o seu valor SAD
    for ( h = 0; h <= heightFrame- sizeBlock; h+=sizeBlock)
    {
        for ( w = 0; w <= widthFrame- sizeBlock; w+=sizeBlock)
        {
            block = getblock(frameR, w, h, sizeBlock);
            blocosCandidatos.push_back(blocoCandidato());
            blocosCandidatos[count].posicao.W = w;
            blocosCandidatos[count].posicao.H = h;
            blocosCandidatos[count].SAD = SAD(sizeBlock,block, blocoAtual);
            deleteMatrix(block,sizeBlock,sizeBlock);
            count++;
        }
    }

    // percorrer vector e retornar x,y do que tem menor SAD
    int menorSAD = blocosCandidatos[0].SAD;
    int posicaoMelhorBloco = 0;

    for (int i = 1; i < blocosCandidatos.size(); i++){
        if (blocosCandidatos[i].SAD < menorSAD){
            posicaoMelhorBloco = i;
            menorSAD = blocosCandidatos[i].SAD;
        }
    }
    return blocosCandidatos[posicaoMelhorBloco].posicao;
}

int main(int argc, char *argv[]) {

  // start MPI
  int qtd_ranks, rank;
  MPI_Status st;
  MPI_Request rq;
  char computador[MPI_MAX_PROCESSOR_NAME];
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &qtd_ranks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int nMyRank = MPI::COMM_WORLD.Get_rank();
  
  printf("Estou executando no computador %s, meu rank %d de um total de %d processos\n", computador, rank, qtd_ranks);
  // start MPI

  char *frameR, *frameA;
  time_t begin = time(NULL);

  if (rank == 0) {    
    
    
    video = fopen("video_converted_640x360.yuv", "rb");
    frameR = readFrames(widthFrame, heightFrame, video);
   
    
    ofstream file;
    file.open("vetoresPosicao2.txt");
    for (int iFrame = 0; iFrame < (numeroFrames-1)/(qtd_ranks-1); iFrame +=1) {     
      
      for(int i=0; i < (qtd_ranks-1); ++i){
        frameA = readFrames(widthFrame, heightFrame, video);
        // MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
        MPI_Send(frameR, char_por_frame, MPI_CHAR, i+1, 0, MPI_COMM_WORLD);
        printf("Enviado frameR numero%d rank %d)\n", iFrame+1,iFrame%(qtd_ranks-1)+1);
        MPI_Send(frameA, char_por_frame, MPI_CHAR, i+1, 1, MPI_COMM_WORLD);
        printf("Enviado frameA numero %d rank %d)\n", iFrame+1,iFrame%(qtd_ranks-1)+1);
        delete (frameR);
        frameR = frameA;
      }
      //recebimento
      
      for(int i=0; i < (qtd_ranks-1); ++i){
        int size;
        vector<int> vetRecebimento;

        printf("Vou receber do rank: %d\n",iFrame%(qtd_ranks-1)+1);
        //(void *buf, int count, MPI_Datatype datatype, int source,int tag, MPI_Comm comm, MPI_Request * request)
        
        MPI_Probe(MPI_ANY_SOURCE, 100, MPI_COMM_WORLD, &st);
        MPI_Get_count(&st , MPI_INT , &size );
        vetRecebimento.resize(size);
        MPI_Recv(vetRecebimento.data(), vetRecebimento.size(), MPI_INT, MPI_ANY_SOURCE, 100, MPI_COMM_WORLD, &st);     
        printf("Recebido do rank: %d\n",iFrame%(qtd_ranks-1)+1);
        file<<"frame "<<iFrame+1<<'\n';
        for(vector<int>::const_iterator i = vetRecebimento.begin(); i != vetRecebimento.end(); ++i) {
          file <<*i<<',';
        }
        file<<'\n';
      }

    }
    delete (frameR);
    file.close();
  }

  else {
               
    for (int iFrame = 0; iFrame < (numeroFrames-1)/(qtd_ranks-1); ++iFrame) {
      
      
      frameR = new char[char_por_frame];
      frameA = new char[char_por_frame];

      // int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
      
      MPI_Recv(frameR, char_por_frame, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &st);
      printf("recebido frame R numero%d rank%d)\n",(iFrame*(qtd_ranks-1))+rank, rank);
      
      MPI_Recv(frameA, char_por_frame, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &st);
      printf("recebido frame A numero %d rank %d)\n",(iFrame*(qtd_ranks-1))+rank, rank);
      
      int count = 0;
      vector<int> vetRecebimento;
      
      #pragma omp parallel for
      for (int h = 0; h <= heightFrame - sizeBlock; h += sizeBlock) { // dividir frame A em blocos sem superposição
        for (int w = 0; w <= widthFrame - sizeBlock; w += sizeBlock) {
          
          char **block = getblock(frameA, w, h, sizeBlock);                                   // pega um bloco sem sobreposição em A
          vetor Rv = findBestBlock(block, frameR, sizeBlock, widthFrame, heightFrame); // retorna o vetor do melhor bloco no frame de referencia
          deleteMatrix(block, sizeBlock, sizeBlock);


          vetRecebimento.push_back(h);				//0 4 8     //   3600*(Frame A(h,w)   FrameR (rv.H,rv.W)  )   
          vetRecebimento.push_back(w);		//1 5 9
          vetRecebimento.push_back(Rv.H);	//2 6 10
          vetRecebimento.push_back(Rv.W);	//3 7 11 
          //printf("Ra(%d,%d),Rv(%d,%d)\n",h,w,Rv.H,Rv.W);
           
        }
      } 
      printf("Vou enviar: %d\n",rank);
      MPI_Send(vetRecebimento.data(), vetRecebimento.size(), MPI_INT, 0, 100, MPI_COMM_WORLD);
      printf("Enviou: %d\n",rank);
      delete (frameR);
      delete (frameA);  
    }
    std::cout << "Processador" << nMyRank << " finalizou\n";
  }
  
  std::cout << "Processador" << nMyRank << " Esperando pela finalização dos outros...\n";
  MPI::COMM_WORLD.Barrier();
  if (rank == 0){ 
      fclose(video);
      time_t end = time(NULL);
      printf("Tempo de execução foi de %ld segundos\n", (end - begin)); 
    }
  
  
  MPI_Finalize();
  return 0;
}

// mpic++ FullSearch_OPENMP+MPI.cpp -o teste
// mpirun --host localhost:4 teste

// mpic++ -fopenmp FullSearch_OPENMP+MPI.cpp -lpthread -o teste


//https://www.cse-lab.ethz.ch/wp-content/uploads/2019/11/hpcse1-19_Lecture_09_distributed_memory_MPI_2.pdf