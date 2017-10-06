#include <QThread>
#include <QObject>
#include <QString>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "microcam.h"
#include "parameters.h"

MicroCam::MicroCam(QObject* CamPU):
    QThread(CamPU),
    deviceID(0),
    apiID(cv::CAP_ANY)
{
    this->webcamUnivers = CamPU;
}

bool MicroCam::initCamera(){

    printf("\nInitialisation MicroCam...");
    cap.open(deviceID + apiID);
    if (!cap.isOpened())
        return false;

    printf("\nTest de capture video...");
    cap >> buffer.frame_0;
    if(buffer.frame_0.empty())
        return false;

    printf("\nSet Frame Rate...");
    if (!cap.set(cv::CAP_PROP_FPS, VC_FREQ))
        return false;
    printf(" %f", VC_FREQ);

    //cap.set(cv::CAP_PROP_BUFFERSIZE,3);

    return true;
}

bool MicroCam::initVideoSave(){
    printf("\nInitialisation VideoSave...");
    int codec = CV_FOURCC('M','J','P','G');
    std::string filename = "./live.avi";
    bool isColor = (buffer.frame_0.type() == CV_8UC3);
    writer.open(filename, codec, VC_FREQ, buffer.frame_0.size(), isColor);
    if(!writer.isOpened())
        return false;
    printf(" %s", filename.c_str());
    return true;
}

void MicroCam::capFrame(){
    switch (buffer.nBuff) {
    case 0:
        cap >> buffer.frame_0;
        pOld = pNew;
        pNew = buffer.frame_3.data;
        break;
    case 1:
        cap >> buffer.frame_1;
        pOld = pNew;
        pNew = buffer.frame_0.data;
        break;
    case 2:
        cap >> buffer.frame_2;
        pOld = pNew;
        pNew = buffer.frame_1.data;
        break;
    case 3:
        cap >> buffer.frame_3;
        pOld = pNew;
        pNew = buffer.frame_2.data;
        break;
    }
    buffer.nBuff = fmod(buffer.nBuff+1,4);
}

void MicroCam::run(){
    tRun = (initCamera() && initVideoSave());
    if(!tRun) printf("\nErreur d'initialisation Caméra !\n");
    else printf("\nLancement de la boucle de capture video !\n");
    while(tRun){
        capFrame();
    }
}
