#include<iostream>
using namespace std;
#include <cstdlib>
char *frameR, *frameA, *Lixo;

FILE *fr;


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

void readFrames(int x,int y){
    
    frameR = (char*) malloc((size_t) (x * y));
    Lixo = (char*) malloc((size_t) (x * y/2));   // 2*(x/2,y/2) =  (x,y/2) 
    frameA = (char*) malloc((size_t) (x * y));

    fr = fopen("akiyo_qcif.yuv", "rb");//Input file
    
    for(int frame = 1; frame <= 2; frame++){
        fread(frameR, (size_t) (x), (size_t) (y), fr);
        fread(Lixo, (size_t) (x), (size_t) (y/2), fr);
        fread(frameA, (size_t) (x), (size_t) (y), fr);
        fread(Lixo, (size_t) (x), (size_t) (y/2), fr);
    }

    char** matrixR;
    char** matrixA;
    
    matrixR = vectorToMatrix(176,144,frameR);

    printMatrix(176,144,matrixR);
    // matrixR = vectorToMatrix(176,144,frameA);
    // printMatrix(176,144,matrixA); 
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


int main(int argc, char *argv[]){
    
    // readFrames(176,144);
    // fclose(fr);
    // system("pause");
    // return 0;

    char** matrizteste = 0;
    char** matrizteste2 = 0;

    char** bloco = 0;

    printf("\n\nMATRIZ TESTE 1:\n");
    matrizteste = gerarMatrizTeste(4,3,'a');
    printMatrix(4,3,matrizteste);
    matrizteste2 = gerarMatrizTeste(4,3,'b');
    printf("\n\nMATRIZ TESTE 2:\n");
    printMatrix(4,3,matrizteste2);
    printf("SAD = %d",SAD(4,3,matrizteste,matrizteste2));
    system("pause");

}


