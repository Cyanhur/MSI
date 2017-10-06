#ifndef MICROCAM_H
#define MICROCAM_H

#include <QtGui>
#include <QObject>
#include <QString>
#include <SDL2/SDL.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

typedef struct _CV_CAM_BUFF
{
    cv::Mat frame_0;
    cv::Mat frame_1;
    cv::Mat frame_2;
    cv::Mat frame_3;
    int nBuff;
} CV_CAM_BUFF;

class MicroCam : public QThread
{
    Q_OBJECT

public:

    MicroCam(QObject* CamPU = NULL);
    //~MicroCam();
    void run();
    //void close();

    bool tRun;

private:

    bool initCamera();
    bool initVideoSave();
    void capFrame();

    //cv::Mat frame;
    cv::VideoCapture cap;
    cv::VideoWriter writer;
    CV_CAM_BUFF buffer;

    uchar *pLast;

    int deviceID;
    int apiID;

    QObject* webcamUnivers;

};

#endif // MICROCAM_H
