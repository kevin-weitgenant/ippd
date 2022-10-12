#include<iostream>
using namespace std;

char *frameR;
char *Lixo, *tabCb;
char *frameA;

FILE *fr, *fw, *fwcb,*fwcr,*fw2;

void processFrames(int x,int y){
    
    frameR = (char*) malloc((size_t) (x * y));
    Lixo = (char*) malloc((size_t) (x * y/2));

    frameA = (char*) malloc((size_t) (x * y));

    fr = fopen("akiyo_qcif.yuv", "rb");//Input file
    
    fw = fopen("frame1.yuv", "wb");
    fwcb = fopen("framecb.yuv", "wb");
    fw2 = fopen("frame2.yuv", "wb");

    
    for(int frame = 1; frame <= 1; frame++){
        fread(frameR, (size_t) (x), (size_t) (y), fr);
        fread(Lixo, (size_t) (x), (size_t) (y/2), fr);
        fread(frameA, (size_t) (x), (size_t) (y), fr);
        

        fwrite(frameR, (size_t) (x * y), 1, fw);
        fwrite(Lixo, (size_t) (x * y/2), 1, fwcb);
        fwrite(frameA, (size_t) (x * y), 1, fw2);
    }
}

int main(int argc, char *argv[]){
    
    processFrames(176,144);
    //Closing the files
    fclose(fr);
    fclose(fw);
    fclose(fwcb);
    fclose(fw2);


    return 0;
}