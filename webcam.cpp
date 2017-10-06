#include <QThread>
#include <QObject>
#include <QString>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <ueye.h>
#include <iostream>
#include <visp3/core/vpImage.h>
#include <visp3/core/vpImageConvert.h>
#include <visp3/io/vpVideoWriter.h>
#include <visp3/core/vpDisplay.h>


#include "webcam.h"
#include "parameters.h"

Webcam::Webcam(QObject* CamPU):
    QThread(CamPU)
{
    this->webcamUnivers = CamPU;
    hCam = (HIDS)0;
    ZeroMemory(m_Images, sizeof(m_Images));

    frame.init(VC_HEIGHT, VC_WIDTH);

    robotPos.x = 0;
    robotPos.y = 0;
}

Webcam::~Webcam()
{
    printf("Fermeture Camera...\n");
    for (unsigned int i = 0; i < sizeof(m_Images) / sizeof(m_Images[0]); i++)
    {
        if (m_Images[i].pBuf)
        {
            if(is_FreeImageMem (hCam, m_Images[i].pBuf, m_Images[i].nImageID)!= IS_SUCCESS)
                checkError();
            m_Images[i].pBuf = NULL;
            m_Images[i].nImageID = 0;
        }
    }
    if(is_ExitCamera(hCam) != IS_SUCCESS) checkError();
}

void Webcam::close(){
    tRun = false;
}

bool Webcam::initCamera(){

    printf("\nInitialisation ueye...");
    if (is_InitCamera(&hCam, NULL) != IS_SUCCESS)
        return false;

    printf("\nColor Mode...");
    switch(bytesPP){
    case 4: if (is_SetColorMode(hCam, IS_CM_RGBA8_PACKED) != IS_SUCCESS) return false; break;
    case 3: if (is_SetColorMode(hCam, IS_CM_RGB8_PACKED) != IS_SUCCESS) return false; break;
    case 1: if (is_SetColorMode(hCam, IS_CM_MONO8) != IS_SUCCESS) return false; break;
    }

    printf("\nAllocation des buffers...");
    for(unsigned int i = 0;i<sizeof(m_Images)/sizeof(m_Images[0]);i++){
        printf(" %d...",i+1);
        if (m_Images[i].pBuf){
            is_FreeImageMem (hCam, m_Images[i].pBuf, m_Images[i].nImageID);
            m_Images[i].pBuf = NULL;
            m_Images[i].nImageID = 0;
        }
        if (is_AllocImageMem (hCam, VC_WIDTH, VC_HEIGHT, bytesPP*8, &m_Images[i].pBuf,
                              &m_Images[i].nImageID) != IS_SUCCESS)
            return false;
        if (is_AddToSequence (hCam, m_Images[i].pBuf,
                              m_Images[i].nImageID) != IS_SUCCESS)
            return false;

        m_Images[i].nImageSeqNum = i + 1;
        m_Images[i].nBufferSize = VC_WIDTH * VC_HEIGHT * bytesPP;
    }

    printf("\nSet Image Format...");
    if(!getFormatList())
        return false;
    int i = 0;
    while (formatList->FormatInfo[i].nFormatID!=27){
        i++;
    }
    formatInfo = formatList->FormatInfo[i];
    if(is_ImageFormat(hCam, IMGFRMT_CMD_SET_FORMAT, &formatInfo.nFormatID, sizeof(formatInfo.nFormatID)) != IS_SUCCESS)
        return false;
    printf(" %s", formatInfo.strFormatName);

    int pixelClock = 32;
    printf("\nSet Pixel Clock...");
    if(is_PixelClock(hCam, IS_PIXELCLOCK_CMD_SET, (void*)&pixelClock, sizeof(pixelClock)) != IS_SUCCESS)
        return false;

    printf("\nExposure Time...");
    double dEnable = 20;
    if (is_Exposure(hCam,IS_EXPOSURE_CMD_SET_EXPOSURE, &dEnable,8) != IS_SUCCESS) return false;
    //if (is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SHUTTER, &dEnable,0) != IS_SUCCESS) return false;

    printf("\nSet Frame Rate...");
    if (is_SetFrameRate(hCam, VC_FREQ, &frameRate) != IS_SUCCESS)
        return false;
    printf(" %f", frameRate);

    printf("\nStart video capture...");
    if (is_SetExternalTrigger(hCam, IS_SET_TRIGGER_OFF) != IS_SUCCESS)
        return false;

    if (is_CaptureVideo(hCam, IS_DONT_WAIT) != IS_SUCCESS)
        return false;

    printf("\n>Ok\n");
    return true;

}

void Webcam::initVideoSave(){
    try{
#if VISP_HAVE_OPENCV_VERSION >= 0x030000
        writer.setCodec( cv::VideoWriter::fourcc('P','I','M','1') ); // MPEG-1 codec
#elif VISP_HAVE_OPENCV_VERSION >= 0x020100
        writer.setCodec( CV_FOURCC('P','I','M','1') );
#endif
        writer.setFramerate(30);
        writer.setFileName("video.mpeg");
        writer.open(frame);
    } catch (vpException e){
        printf("Erreur lors de l'initialisation de la sauvegarde video : %s",e);
    }
}

bool Webcam::getFormatList(){
    // Get number of available formats and size of list
    UINT bytesNeeded = sizeof(IMAGE_FORMAT_LIST);
    nRet = is_ImageFormat(hCam, IMGFRMT_CMD_GET_NUM_ENTRIES, &count, sizeof(count));
    bytesNeeded += (count - 1) * sizeof(IMAGE_FORMAT_INFO);
    void* ptr = malloc(bytesNeeded);

    // Create and fill list
    formatList = (IMAGE_FORMAT_LIST*) ptr;
    formatList->nSizeOfListEntry = sizeof(IMAGE_FORMAT_INFO);
    formatList->nNumListElements = count;
    if(is_ImageFormat(hCam, IMGFRMT_CMD_GET_LIST, formatList, bytesNeeded) != IS_SUCCESS)
        return false;
    return true;
}

void Webcam::checkError(){
    INT pErr;
    IS_CHAR* ppcErr;
    is_GetError(hCam, &pErr, &ppcErr);
    printf("%s\n", ppcErr);
}

void Webcam::imgConvert(char* input){
    {
        char *pt_input = input;
        char* pt_end = input + VC_HEIGHT*VC_WIDTH*bytesPP;
        unsigned char *pt_output = frame.bitmap;

        while(pt_input != pt_end) {
            *pt_output++ = (unsigned char)((*pt_input)+1);
            pt_input += bytesPP;
        }
    }
}

void Webcam::videoSave(){
    try{
        vpDisplay::getImage(frame, sframe);
        if(overlayCalib){
            drawRect(mmcArena.arenaBorder,'r');
            for(int i=0;i<28;i++){
                if(mmcArena.dot[i].act == false){
                    drawCircle(mmcArena.dot[i].pos,mmcArena.radius,'b');
                } else {
                    drawCircle(mmcArena.dot[i].pos,mmcArena.radius,'g');
                }
            }
        }
        writer.saveFrame(sframe);
    } catch(vpException e){
        printf("Erreur lors de l'enregistrement video : %s",e);
    }
}

void Webcam::drawPoint(int x, int y,char color){
    int R = 0, G = 0, B = 0;
    switch (color){
    case'r':R=255; break;
    case'g':G=255; break;
    case'b':B=255; break;
    }
    sframe[y][x].R=R;
    sframe[y][x].G=G;
    sframe[y][x].B=B;
}

void Webcam::drawCircle(SDL_Point pos, int radius, char color){
    for(int t = 0; t<359;t++){
        drawPoint(pos.x+radius*cos(t/PI),pos.y+radius*sin(t/PI),color);
    }
}

void Webcam::drawRect(SDL_Rect rect, char color){
    for(int i = rect.x; i < rect.x + rect.w; i++){
        drawPoint(i,rect.y,color);
        drawPoint(i,rect.y+rect.h,color);
    }
    for(int j = rect.y; j< rect.y + rect.h; j++){
        drawPoint(rect.x,j,color);
        drawPoint(rect.x + rect.w,j,color);
    }
}

bool Webcam::trackLost(){
    try {
        printf(".Tentative de récupération sur le buffer actuel...\n");
        imgConvert(pNew);
        blob.track(frame,germ);
        return false;
    } catch (...){
        try {
            printf("..Tentative de récupération sur le buffer précédent...\n");
            imgConvert(pOld);
            blob.track(frame,germ);
            return false;
        } catch(...){
            return true;
        }
    }
}



void Webcam::run(){
    tRun = initCamera();
    if(!tRun) checkError();
    initVideoSave();
    while(tRun){
        is_GetActSeqBuf(hCam, &dummy, &pMem, &pLast);
        if(pLast != pNew){
            pOld = pNew;
            pNew = pLast;
            imgConvert(pLast);
            videoSave();
            if(trackingOn){
                if (!trackingStarted && mousePos.x != -1){
                    germ.set_uv((double)mousePos.x, (double)mousePos.y);
                    printf("Position du germe : %f x %f\n", germ.get_u(), germ.get_v());
                    try{
                        blob.initTracking(frame,germ);
                        trackingStarted = true;
                        germ = blob.getCog();
                        printf("Début du tracking\n");
                        blob.setGrayLevelMin(0);
                        //blob.setGrayLevelMax(blob.getMeanGrayLevel()+50);
                        //blob.setGrayLevelPrecision(0.5);
                        blob.setEllipsoidShapePrecision(0);
                        printf("Position du robot : %f x %f\n",blob.getCog().get_u(), blob.getCog().get_v());
                    }
                    catch(...){
                        printf("Erreur d'initialisation du tracking\n");
                        trackingOn = false;
                    }
                }
                if (trackingStarted){
                    try {
                        blob.track(frame, germ);
                        //blob.setGrayLevelMax(blob.getMeanGrayLevel()+50);
                        robotPos.x = (int)germ.get_u();
                        robotPos.y = (int)germ.get_v();
                    } catch(...){
                        printf("Perte du tracking...\n");
                        if(!trackLost()){
                            robotPos.x = (int)germ.get_u();
                            robotPos.y = (int)germ.get_v();
                        } else {
                            trackingOn = false;
                            trackingStarted = false;
                            printf("Echec des tentatives de récupération...\nFin du tracking...\n");
                        }
                    }
                }
            }
        }
    }
}

