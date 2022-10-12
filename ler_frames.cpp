#include<iostream>
using namespace std;

char *taby1;
char *tabLixo, *tabCb;
char *taby2;

FILE *fr, *fw, *fwcb,*fwcr,*fw2;

void processFrames(int x,int y){
    
    taby1 = (char*) malloc((size_t) (x * y));
    tabLixo = (char*) malloc((size_t) (x * y/2));

    taby2 = (char*) malloc((size_t) (x * y));

    fr = fopen("akiyo_qcif.yuv", "rb");//Input file
    
    fw = fopen("frame1.yuv", "wb");
    fwcb = fopen("framecb.yuv", "wb");
    fw2 = fopen("frame2.yuv", "wb");

    
    for(int frame = 1; frame <= 1; frame++){
        fread(taby1, (size_t) (x), (size_t) (y), fr);
        fread(tabLixo, (size_t) (x), (size_t) (y/2), fr);
        fread(taby2, (size_t) (x), (size_t) (y), fr);
        

        fwrite(taby1, (size_t) (x * y), 1, fw);
        fwrite(tabLixo, (size_t) (x * y/2), 1, fwcb);
        fwrite(taby2, (size_t) (x * y), 1, fw2);
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