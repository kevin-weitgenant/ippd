// #include<opencv2/opencv.hpp>
// using namespace cv;
#include<iostream>
using namespace std;



void processFrames(int x,int y){
    FILE *fr, *fw;
    char frame[x][y];

    taby1 = (char*) malloc((size_t) (x * y));
    tabCr = (char*) malloc((size_t) (x/2 * y/2));
    tabCb = (char*) malloc((size_t) (x/2 * y/2));

    taby2 = (char*) malloc((size_t) (x * y));

    fr = fopen("akiyo_qcif.yuv", "rb");//Input file
    
    fw = fopen("frame1.yuv", "wb");
    fwcb = fopen("framecb.yuv", "wb");
    fwcr = fopen("framecr.yuv", "wb");
    fw2 = fopen("frame2.yuv", "wb");

    
    for(int frame = 1; frame <= 1; frame++){
        fread(taby1, (size_t) (x), (size_t) (y), fr);
        fread(tabCr, (size_t) (x/2), (size_t) (y/2), fr);
        fread(tabCb, (size_t) (x/2), (size_t) (y/2), fr);
        fread(taby2, (size_t) (x), (size_t) (y), fr);
        

        fwrite(taby1, (size_t) (x * y), 1, fw);
        fwrite(tabCr, (size_t) (x * y), 1, fwcb);
        fwrite(tabCb, (size_t) (x * y), 1, fwcr);
        fwrite(taby2, (size_t) (x * y), 1, fw2);
    }
}

int main(int argc, char *argv[]){
    
    processFrames(176,144);
    //Closing the files
    fclose(fr);
    fclose(fw);
    fclose(fwcb);
    fclose(fwcr);
    fclose(fw2);


    return 0;
}