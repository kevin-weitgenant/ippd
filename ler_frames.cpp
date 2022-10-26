#include<iostream>
using namespace std;
#include <cstdlib>


FILE *video;


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

char * readFrames(int width,int heigth, FILE *video){
    char *frameY, *Lixo;

    frameY = (char*) malloc((size_t) (width * heigth));
    Lixo = (char*) malloc((size_t) (width * heigth/2));   // 2*(x/2,y/2) =  (x,y/2) 

    
    fread(frameY, (size_t) (width), (size_t) (heigth), video);
    fread(Lixo, (size_t) (width), (size_t) (heigth/2), video);
    
    
    //printMatrix(width,heigth, vectorToMatrix(width,heigth,frameY));
    
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
	
int SAD(int width,int height,char** block1,char **block2){
    int result = 0;
    
    for (int h = 0; h < height; h++){
      
        for (int w = 0; w < width; w++){
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


char **genSearchWindow(char **blocoReferencia,int sizeBlock, int sizeWindow,int pixAtualHeigth, int pixAtualWidth )


char **convolution(char **blocoAtual, int sizeBlock, int sizeWindow,int pixAtualHeigth, int pixAtualWidth){  //retorna a posição do bloco dentro da searchWindow que 
    int i = 0;                                                                              //tem o menor SAD
    int j = 0;
    int count = 0;

    char **bloco8x8;
    
    for ( i = 0; i <= sizeWindow- sizeBlock; i++)
    {
        for ( j = 0; j < sizeWindow- sizeBlock; j++)
        {
            printf("\n\nBloco 8x8 do pixel[%d][%d]\n", i , j);
            bloco8x8 = getblock(searchWindow, i, j, 8);
            printMatrix(8,8, bloco8x8);
            
            //SAD AQUI
            SAD(8,8,bloco8x8, blocoAtual);



        }
 
    }
}


int main(int argc, char *argv[]){
    char *frameR, *frameA;
    video = fopen("akiyo_qcif.yuv", "rb");//Input file

    int numeroFrames = 300;

    for (int i = 0; i< numeroFrames-1; i++){
        frameR = readFrames(176,144,video);
        frameA = readFrames(176,144,video);
        
    //     >>compareFrames(frameR,frameA) >>> 
    // dividir frame A em blocos 8x8(os blocos não tem sobreposição, usar a função getBlock8x8 dando as posições certas) 
    // para cada bloco criado, chamar convolution 


    }

    fclose(video);
    system("pause");
    return 0;

    // char** matrizteste = 0;
    // char** matrizteste2 = 0;

    // char** bloco = 0;

    // printf("\n\nMATRIZ TESTE 1:\n");
    // matrizteste = gerarMatrizTeste(4,3,'a');
    // printMatrix(4,3,matrizteste);
    // matrizteste2 = gerarMatrizTeste(4,3,'b');
    // printf("\n\nMATRIZ TESTE 2:\n");
    // printMatrix(4,3,matrizteste2);
    // printf("SAD = %d",SAD(4,3,matrizteste,matrizteste2));
    // system("pause");

}


