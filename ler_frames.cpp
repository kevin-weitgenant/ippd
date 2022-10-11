// #include<opencv2/opencv.hpp>
// using namespace cv;
#include<iostream>
using namespace std;

char *taby;
FILE *fr, *fw;

// int getNumberOfFrames(video){

//    VideoCapture cap("video.mp4");//Declaring an object to capture stream of frames from default camera//
//    frame_Number = cap.get(CAP_PROP_FRAME_COUNT);//Getting the total number of frames//
//    cout << "Total Number of frames are:" << frame_Number << endl;//Showing the number in console window//
//    system("pause");//Pausing the system to see the result//
//    cap.release();//Releasing the buffer memory//
//    return 0;
// }


void processFrames(int x,int y){
    
    
    taby = (char*) malloc((size_t) (x * y));
    fr = fopen("video_converted_640x360.yuv", "rb");//Input file
    fw = fopen("teste.yuv", "wb");
    
    for(int frame = 1; frame <= 1; frame++){
        fread(taby, (size_t) (x / 2), (size_t) (y * 2), fr);
        fwrite(taby, (size_t) (x * y), 1, fw);
    }
}

int main(int argc, char *argv[]){
    
    processFrames(640,360);
    //Closing the files
    fclose(fr);
    fclose(fw);
    return 0;
}