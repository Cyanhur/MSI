#ifndef WEBCAM_H
#define WEBCAM_H

#include <QtGui>
#include <QObject>
#include <QString>
#include <SDL2/SDL.h>
#include <ueye.h>
#include <visp3/core/vpImage.h>
#include <visp3/blob/vpDot2.h>
#include <visp3/io/vpVideoWriter.h>

typedef struct _UEYE_IMAGE
{
    char *pBuf;
    INT nImageID;
    INT nImageSeqNum;
    INT nBufferSize;
} UEYE_IMAGE, *PUEYE_IMAGE;

class Webcam : public QThread
{
    Q_OBJECT
public:

    Webcam(QObject* CamPU = NULL);
    ~Webcam();
    void run();
    void close();

private:

    bool initCamera();
    void initVideoSave();
    bool getFormatList();
    void imgConvert(char* input);
    void videoSave();
    void drawPoint(int x, int y,char color);
    void drawCircle(SDL_Point pos, int radius, char color);
    void drawRect(SDL_Rect rect, char color);
    void checkError();
    bool trackLost();

    HIDS hCam;
    INT nRet;
    UINT count;
    IMAGE_FORMAT_INFO formatInfo;
    IMAGE_FORMAT_LIST* formatList;
    UEYE_IMAGE m_Images[5];
    double frameRate;

    char *pMem, *pLast;
    unsigned char *pGrey;
    INT dummy, old_dummy;

    vpImage<unsigned char> frame;
    vpImage<vpRGBa> sframe;
    vpImagePoint germ;
    vpDot2 blob;
    vpVideoWriter writer;
    int greyLevel;

    QObject* webcamUnivers;

    bool tRun;
};

#endif // WEBCAM_H
