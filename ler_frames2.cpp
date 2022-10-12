#include<iostream>
using namespace std;

char *frameR, *frameA, *Lixo;

FILE *fr;

void printFrame(char *frame,int x,int y){
    char position;

    for (int i =0; i<x; i++){
        printf("\n");
        for (int j =0; j<y; j++)
            position = *(frame +(i*j + j)) ;
            cout << position;
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

    printf("Primeiro frame:\n\n\n\n");
    printFrame(frameR, x, y);
    printf("\n\nSegundo frame:\n\n\n\n");
    printFrame(frameA, x, y);
}

int main(int argc, char *argv[]){
    
    readFrames(176,144);
    //Closing the files
    fclose(fr);
    return 0;
}