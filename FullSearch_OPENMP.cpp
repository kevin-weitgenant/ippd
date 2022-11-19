#include<iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <time.h>
#include <omp.h>
#include "mpi.h"

FILE *video;


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

int const numeroFrames = 120;
int const heightFrame = 360;
int const widthFrame = 640;
int const sizeBlock = 8;


char acessFrameArrayAsMatrix(char *vetor, int h, int w){
    return vetor[widthFrame*h + w];
}

void deleteMatrix(char **matrix,int width,int heigth){

    for( int h = 0 ; h < heigth ; h++ )
    {
        delete[] matrix[h]; // delete array within matrix
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
    for ( h = 0; h <= heightFrame- sizeBlock; h+= 1)
    {
        for ( w = 0; w <= widthFrame- sizeBlock; w+=1)
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

int main(int argc, char *argv[]){
    
    char *frameR, *frameA;

    //start MPI
    int qtd_ranks, rank;
    MPI_Status st;
    char computador[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &qtd_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("Estou executando no computador %s, meu rank %d de um total de %d processos\n", computador, rank, qtd_ranks);
	//start MPI

    //variáveis pra lógica do MPI 
  	int char_por_frame = widthFrame* heightFrame;
    int num_exec_for_for = ( (widthFrame - sizeBlock) / sizeBlock ) * ((heightFrame - sizeBlock) / sizeBlock );
    int* vetorResultado = new int[4*num_exec_for_for];
  	//variáveis pra lógica do MPI 

    time_t begin;
    if (rank ==0){  //master envia frame pra todos os processadores
        begin = time(NULL);
        
        video = fopen("video_converted_640x360.yuv", "rb");// só master 0 lê o arquivo
        frameR = readFrames(widthFrame,heightFrame,video); 

        //for envio
        // processador1 RECEBE frame 0,1
        // processador2 RECEBE frame 1,2   
        for (int iFrame = 0; iFrame< numeroFrames-1; iFrame += qtd_ranks - 1){	//numero de quadros total = quadrostotal/n_processador
            for(int rank_destino = 1; rank_destino < qtd_ranks; rank_destino++){
                printf("Enviado requisição de frameR %d", rank_destino);
                MPI_Send(&frameR, char_por_frame, MPI_CHAR, rank_destino, MPI_ANY_TAG, MPI_COMM_WORLD);  
                //MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
                frameA = readFrames(widthFrame,heightFrame,video);
                printf("Enviado requisição de frameA %d", rank_destino);
                MPI_Send(&frameA, char_por_frame, MPI_CHAR, rank_destino, MPI_ANY_TAG, MPI_COMM_WORLD);  
                //colocar uma tag indentificando que é frame R ou A?????????????????

                frameR = frameA; //agora o proximo ref é o atual
                
            }
        }

        //for recebimento
        //#pragma for
        for (int iFrame = 0; iFrame< numeroFrames-1; iFrame += qtd_ranks - 1){
            for(int rank_remetente = 1; rank_remetente < qtd_ranks; rank_remetente++){  
                MPI_Recv(vetorResultado, 4*num_exec_for_for, MPI_INT, rank_remetente, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
                printf("%d\n", vetorResultado[0]);
                //delete(vetorResultado);
            }
        }

    }

    else{ //cada processador escravo executa:      
        time_t beginFrame = time(NULL);
        int index;
        
        for (int iFrame = 0; iFrame< numeroFrames-1; iFrame += qtd_ranks - 1){      //recebendo a mesma quantidade que foi enviado
            MPI_Recv(frameR, char_por_frame, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st); //ver se Recv recebe em ordem certa
            MPI_Recv(frameA, char_por_frame, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &st); //ver se Recv recebe em ordem certa

            printf("\n\n FRAME %dn\n",iFrame+1);
            
            //#pragma omp parallel for collapse(2)
            for ( int h = 0; h <= heightFrame- sizeBlock; h+=sizeBlock){//dividir frame A em blocos sem superposição  
                for ( int w = 0; w <= widthFrame- sizeBlock; w+=sizeBlock){         
                    char **block;
                    block = getblock(frameA, w, h, sizeBlock); //pega um bloco sem sobreposição em A
                    vetor Rv = findBestBlock(block, frameR,sizeBlock,widthFrame, heightFrame); //retorna o vetor do melhor bloco no frame de referencia
                    deleteMatrix(block,sizeBlock,sizeBlock);
                    index = ( h* ( (widthFrame - sizeBlock) / sizeBlock ) + w )*4;
                    vetorResultado[index] = h;				//0 4 8
                    vetorResultado[1+index] = w;			//1 5 9
                    vetorResultado[2+index] = Rv.H;	//2 6 10
                    vetorResultado[3+index] = Rv.W;	//3 7 11    
                }
            }  
        //MPI_Send(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
        MPI_Send(&vetorResultado, 4*num_exec_for_for, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);   
      }
        time_t endFrame = time(NULL);
        printf("\n\nTEMPO PROCESSANDO 1 FRAME %ld seconds\n\n", (endFrame - beginFrame));
        //free(frameR);
        //free(frameA);
    }

    if (rank == 0){ 
      fclose(video);
      time_t end = time(NULL);
      printf("The elapsed time is %ld seconds", (end - begin));
      system("pause"); 
    }


    MPI_Finalize();
	return 0;
}



    

// g++ -fopenmp FullSearch_OPENMP.cpp -lpthread -o teste