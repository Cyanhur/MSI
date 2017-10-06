#include <iostream>
#include <QThread>
#include <QObject>
#include <QString>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "graphic.h"
#include "parameters.h"

Graphic::Graphic(QObject* GraphicPU):
    QThread(GraphicPU)
{
    this->graphicUnivers = GraphicPU;
    pFrame = NULL;
    calibCheck = false;
}

Graphic::~Graphic()
{
    printf("Fermeture Interface Graphique...\n");
    if(vTexture != NULL){
        SDL_DestroyTexture(vTexture);
        vTexture = NULL;
    }
    if(gRenderer != NULL){
        SDL_DestroyRenderer(gRenderer);
        gRenderer = NULL;
    }
    if(gWindow != NULL){
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
    }
}

void Graphic::close(){
    tRun = false;
}

bool Graphic::initSDL(){
    printf("\nCréation de la fenêtre...");
    gWindow = SDL_CreateWindow("MSI alpha", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SC_WIDTH, SC_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
        return false;

    printf("\nCréation du renderer...");
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL)
        return false;

    printf("\nCréation de la texture de stream...");
    switch(bytesPP){
    case 4: vTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, VC_WIDTH, VC_HEIGHT); break;
    case 3: vTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, VC_WIDTH, VC_HEIGHT); break;
    case 1: vTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_INDEX8, SDL_TEXTUREACCESS_STREAMING, VC_WIDTH, VC_HEIGHT); break;
    }
    if (vTexture == NULL)
        return false;

    printf("\nCalibrations diverses...");
    SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0);
    printf("\n>Ok\n");
    return true;

}

bool Graphic::initOverlay(){
    if(!calibCheck){
        if(mousePos.x != -1){
            mmcArena.arenaBorder.x = mousePos.x;
            mmcArena.arenaBorder.y = mousePos.y;
            printf("angle supérieur gauche positionné en : %d x %d \nVeuillez cliquer sur l'angle inférieur droit de l'arène\n",mmcArena.arenaBorder.x,mmcArena.arenaBorder.y);
            mousePos.x = -1;
            calibCheck = true;

        } else {
            return false;
        }
    } else if( mousePos.x != -1){
        mmcArena.arenaBorder.w = mousePos.x-mmcArena.arenaBorder.x;
        mmcArena.arenaBorder.h = mousePos.y-mmcArena.arenaBorder.y;
        printf("Dimensions de l'arène : %d x %d pixels\n",mmcArena.arenaBorder.w,mmcArena.arenaBorder.h);
        mousePos.x = -1;
        mmcArena.ratioPixMM = (float)mmcArena.arenaBorder.w/3250;
        printf("Ratio Pixel/µm : %f\n", mmcArena.ratioPixMM);
        mmcArena.radius = 125*mmcArena.ratioPixMM;
        mmcArena.n = 0;
        printf("Précision dimentionnelle de l'overlay : %f\n", (float)mmcArena.arenaBorder.w/mmcArena.arenaBorder.h/1.625);
        for (unsigned int i = 0; i<28;i++){
            mmcArena.order[i] = 0;
            mmcArena.dot[i].act = false;
            mmcArena.dot[i].pos.x = mmcArena.arenaBorder.x + mmcArena.ratioPixMM*(125 + floor(i/4)*500);
            if((i/4)%2 == 0)
                mmcArena.dot[i].pos.y = mmcArena.arenaBorder.y + mmcArena.ratioPixMM*(125 + i%4*500);
            else
                mmcArena.dot[i].pos.y = mmcArena.arenaBorder.y + mmcArena.ratioPixMM*(375 + i%4*500);
        }
        calibCheck = false;
        return true;
    }
    return false;
}

void Graphic::frameLoader(){
    if(pFrame != NULL){
        printf("Texture déjà bloqué !\n");
        return;
    }
    if(SDL_LockTexture(vTexture, NULL, &pFrame, &pPitch)){
        printf("Impossible de bloquer la texture : %s\n", SDL_GetError());
        return;
    }
    memcpy(pFrame, pNew, pPitch*VC_HEIGHT);
    SDL_UnlockTexture(vTexture);
    pBuff = pNew;
    pFrame = NULL;
    pPitch = 0;
}

void Graphic::showCrossMark(){
    SDL_SetRenderDrawColor(gRenderer, 0,0,0xff,0xff);
    SDL_RenderDrawLine(gRenderer, comPos.x, comPos.y-10, comPos.x, comPos.y+10);
    SDL_RenderDrawLine(gRenderer, comPos.x-10, comPos.y, comPos.x+10, comPos.y);
    SDL_SetRenderDrawColor(gRenderer, 0,0xff,0,0xff);
    SDL_RenderDrawLine(gRenderer, mmcArena.dot[mmcArena.order[1]].pos.x, mmcArena.dot[mmcArena.order[1]].pos.y-10, mmcArena.dot[mmcArena.order[1]].pos.x, mmcArena.dot[mmcArena.order[1]].pos.y+10);
    SDL_RenderDrawLine(gRenderer, mmcArena.dot[mmcArena.order[1]].pos.x-10, mmcArena.dot[mmcArena.order[1]].pos.y, mmcArena.dot[mmcArena.order[1]].pos.x, mmcArena.dot[mmcArena.order[1]].pos.y);
    SDL_SetRenderDrawColor(gRenderer,0xff,0xff,0xff,0xff);
    SDL_RenderDrawLine(gRenderer, mousePos.x, mousePos.y-10, mousePos.x, mousePos.y+10);
    SDL_RenderDrawLine(gRenderer, mousePos.x-10, mousePos.y, mousePos.x+10, mousePos.y);
}

void Graphic::showAngles(){
    SDL_SetRenderDrawColor(gRenderer, 0xff,0xff,0,0xff);
    SDL_RenderDrawLine(gRenderer, 50, SC_HEIGHT-50,(50+40*cos(yawAngle+rollBias)),SC_HEIGHT-(50+40*sin(yawAngle+rollBias)));
    SDL_SetRenderDrawColor(gRenderer, 0xff,0,0,0xff);
    SDL_RenderDrawLine(gRenderer, SC_WIDTH-50,SC_HEIGHT-50, SC_WIDTH-(50+40*cos(pitchAngle)), SC_HEIGHT-(50+40*sin(pitchAngle)));
    SDL_RenderDrawLine(gRenderer, 50, SC_HEIGHT-50,(50+40*cos(yawAngle)),SC_HEIGHT-(50+40*sin(yawAngle)));
    SDL_SetRenderDrawColor(gRenderer, 0xff,0xff,0xff,0xff);
}

void Graphic::showFrequency(){

}

void Graphic::run(){
    tRun = initSDL();
    if(!tRun)printf("%s\n", SDL_GetError());
    while(tRun){
        if(pBuff != pNew){
            SDL_RenderClear(gRenderer);
            frameLoader();
            SDL_RenderCopy(gRenderer, vTexture, NULL, NULL);
            if(trackingOn){
                if(trackingStarted){
                    SDL_SetRenderDrawColor(gRenderer, 0xff, 0, 0, 0);
                    SDL_RenderDrawLine(gRenderer, robotPos.x, robotPos.y-10, robotPos.x, robotPos.y+10);
                    SDL_RenderDrawLine(gRenderer, robotPos.x-10, robotPos.y, robotPos.x+10, robotPos.y);
                    SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0);
                } else {

                }
            }
            if(mmcOn){
                if(!overlayCalib){
                    overlayCalib = initOverlay();
                } else {
                    SDL_SetRenderDrawColor(gRenderer,0xff,0,0,0x80);
                    SDL_RenderDrawRect(gRenderer,&mmcArena.arenaBorder);
                    for(unsigned int i = 0; i<28; i++){
                        if(mmcArena.dot[i].act)
                            SDL_SetRenderDrawColor(gRenderer,0,0xff,0,0x80);
                        else
                            SDL_SetRenderDrawColor(gRenderer,0,0,0xff,0x80);
                        for(unsigned int j = 0; j<359;j++){
                            SDL_RenderDrawPoint(gRenderer, mmcArena.dot[i].pos.x+mmcArena.radius*cos(j/PI),mmcArena.dot[i].pos.y+mmcArena.radius*sin(j/PI));
                        }
                    }
                    SDL_SetRenderDrawColor(gRenderer,0xff,0xff,0xff,0);
                }
            }
            showCrossMark();
            showAngles();
            SDL_RenderPresent(gRenderer);
        }
    }
}

