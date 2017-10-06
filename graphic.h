#ifndef GRAPHIC_H
#define GRAPHIC_H

#include <QtGui>
#include <SDL2/SDL.h>

class Graphic : public QThread
{
    Q_OBJECT
public:

    Graphic(QObject* GraphicPU = NULL);
    ~Graphic();
    void run();
    void close();


private:

    bool initSDL();
    bool initOverlay();
    void frameLoader();

    void showCrossMark();
    void showAngles();
    void showFrequency();

    QObject* graphicUnivers;
    SDL_Window* gWindow;
    SDL_Renderer* gRenderer;
    SDL_Texture* vTexture;

    void* pFrame;
    uchar* pBuff;
    int pPitch;

    bool tRun;
    bool calibCheck;
};

#endif // GRAPHIC_H
