#include<iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <time.h>

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


char** vectorToMatrix(int width, int height, char *frame){
      
    char** matrix = 0;
    matrix = new char*[height];
    int iFrame = 0;
    
    for (int h = 0; h < height; h++)
    {
        matrix[h] = new char[width];
        for (int w = 0; w < width; w++)
        {    
                matrix[h][w] = frame[iFrame];
                iFrame++;
        }
    }
    free(frame);
    
    return matrix;
    }

void printMatrix(int width, int height,char** matrix){
    
    for (int h = 0; h < height; h++){
      
            for (int w = 0; w < width; w++){
            
                  printf("%d,", (unsigned int)matrix[h][w]);
            }
            printf("\n\n\n");
      }
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



void writeFrame(int width, int height, char *frameY, char *fileName){
    FILE *fileFrameY = fopen(fileName, "wb");//Input file

    fwrite(frameY, (size_t) (width * height), 1, fileFrameY);

    fclose(fileFrameY);
}


char ** getblock(char ** matrix,int pixelX,int pixelY,int size){
    int hBlock =0;
    int wBlock = 0;
    char** block = 0;
    block = new char*[size];
    
    for (int h = pixelY; h < pixelY+size; h++){
        block[hBlock] = new char[size];
        wBlock = 0;
        for (int w = pixelX; w < pixelX+size; w++){
            block[hBlock][wBlock]= matrix[h][w];
            wBlock++;
        }
        hBlock++;          
    }

    return block;
}
	
int SAD(int sizeblock,char** block1,char **block2){
    int result = 0;
    
    for (int h = 0; h < sizeblock; h++){
      
        for (int w = 0; w < sizeblock; w++){
            result += abs(block1[h][w]- block2[h][w]);
        }
    }
    return result;
} 


char** gerarMatrizTeste(int width, int height, char valor){
    
    char** matrix = 0;
    matrix = new char*[height];
    char i = valor;

    for (int h = 0; h < height; h++){
        matrix[h] = new char[width];
        for (int w = 0; w < width; w++){
                matrix[h][w] = i;
                i++;
        }
    }    
    return matrix;
}

vetor findBestBlock(char **blocoAtual, char **frameR,int sizeBlock, int widthFrame,int heightFrame){  
                                                                        //retorna a posição do bloco dentro do frame(MELHORAR PRA JANELA SE DER TEMPO)
                                                                        //que tem o menor SAD
    int h = 0;                                                                              
    int w = 0;                                                                           
    int blocoCount = 0;
    char **block;
    vector<blocoCandidato> blocosCandidatos;
    int count = 0;

    // esses 2 FOR, adicionam no vetor blocosCandidatos, vários blocos armazenando a posição e o seu valor SAD
    for ( h = 0; h <= heightFrame- sizeBlock; h++)
    {
        for ( w = 0; w <= widthFrame- sizeBlock; w++)
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
    time_t begin = time(NULL);

    int numeroFrames = 120;
    int heightFrame = 360;
    int widthFrame = 640;
    int sizeBlock = 8;
    
    char **matrizFrameA, **matrizFrameR;
    char *frameR, *frameA;

    
    video = fopen("video_converted_640x360.yuv", "rb");//Input file
    frameR = readFrames(widthFrame,heightFrame,video); 
    matrizFrameR = vectorToMatrix(widthFrame,heightFrame,frameR);

    for (int iFrame = 0; iFrame< numeroFrames-1; iFrame++){      //percorrer todos os frames   
        time_t beginFrame = time(NULL);
        frameA = readFrames(widthFrame,heightFrame,video);
        matrizFrameA = vectorToMatrix(widthFrame,heightFrame,frameA);
        int count = 0;
        printf("\n\n FRAME %dn\n",iFrame+1);
        #pragma omp parallel for
        for ( int h = 0; h <= heightFrame- sizeBlock; h+=sizeBlock){//dividir frame A em blocos sem superposição  
            for ( int w = 0; w <= widthFrame- sizeBlock; w+=sizeBlock){         
                char **block;
                block = getblock(matrizFrameA, w, h, sizeBlock); //pega um bloco sem sobreposição em A
                

                vetor Rv = findBestBlock(block, matrizFrameR,sizeBlock,widthFrame, heightFrame); //retorna o vetor do melhor bloco no frame de referencia
                deleteMatrix(block,sizeBlock,sizeBlock);
                if (Rv.W != w || Rv.H != h){
                    printf("Ra(%d,%d),Rv(%d,%d)\n",h,w,Rv.H,Rv.W);
                }
            }
        }
        deleteMatrix(matrizFrameR,widthFrame,heightFrame);
        matrizFrameR = matrizFrameA;   
        time_t endFrame = time(NULL);
        printf("\n\nTEMPO PROCESSANDO 1 FRAME %ld seconds\n\n", (endFrame - beginFrame));
    }
    deleteMatrix(matrizFrameR,widthFrame,heightFrame);
    

    fclose(video);
    time_t end = time(NULL);
    printf("The elapsed time is %d seconds", (end - begin));
    system("pause");
    return 0;

}
