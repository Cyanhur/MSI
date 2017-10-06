#include <iostream>
#include <SDL2/SDL.h>
#include <map>
#include <math.h>

#include "evenements.h"
#include "parameters.h"
#include "arduino.h"
#include "graphic.h"
#include "microcam.h"

void managEvents (Input *in, MicroCam *camera, Graphic *window){
    if(!clCalibration){
        if(in->key[SDLK_t] && !trackingOn && !in->key[SDLK_LCTRL]){
            mousePos.x = -1;
            trackingStarted = false;
            trackingOn = true;
            printf("Cliquer sur le robot pour initialiser le tracking\n");
        }
        if(in->key[SDLK_LCTRL] && in->key[SDLK_t] && trackingOn){
            trackingOn = false;
            mousePos.x = -1;
            printf("Fin du tracking\n");
        }
        if(mmcOn){
            if(!clStart){
                if(in->mouseButtons[SDL_BUTTON_LEFT]){mousePos.x = in->mouseX; mousePos.y = in->mouseY;SDL_Delay(100);}
                if (in->key[SDLK_m]){
                    printf("Calibration de l'overlay, veuillez cliquer sur l'angle supérieur gauche de l'arène\n");
                    mousePos.x = -1;
                    mmcOn = true;
                    overlayCalib = false;
                    mmcArena.n = 0;
                    SDL_Delay(100);
                }
                if (in->key[SDLK_m] && in->key[SDLK_LCTRL]){
                    mmcOn = false;
                    overlayCalib = false;
                    mmcArena.n = 0;
                }
                if (overlayCalib){
                    if(in->mouseButtons[SDL_BUTTON_LEFT]){
                        for(int i = 0; i<28; i++){
                            if(sqrt(pow((in->mouseX - mmcArena.dot[i].pos.x),2) + pow((in->mouseY - mmcArena.dot[i].pos.y),2)) < mmcArena.radius){
                                mmcArena.dot[i].act = true;
                                mmcArena.order[mmcArena.n] = i;
                                mmcArena.n++;
                                printf("nbr point(s) : %d\n", mmcArena.n);
                            }
                        }
                    }
                    if(in->mouseButtons[SDL_BUTTON_RIGHT]){
                        for(int i = mmcArena.n; i>=0; i--){
                            if(sqrt(pow((in->mouseX - mmcArena.dot[mmcArena.order[i]].pos.x),2) + pow((in->mouseY - mmcArena.dot[mmcArena.order[i]].pos.y),2)) < mmcArena.radius){
                                mmcArena.dot[mmcArena.order[i]].act = false;
                                for(int j = i; j < mmcArena.n; j++){
                                    mmcArena.order[j] = mmcArena.order[j+1];
                                }
                                mmcArena.n--;
                            }
                        }
                    }
                    if(in->key[SDLK_s]){
                        printf("MMC Close loop : start\n");
                        spinDirection = 1;
                        clStart = true;
                    }
                }
            } else {
                if(in->key[SDLK_LCTRL] && in->key[SDLK_c]){
                    clStart = false;
                    spinDirection = 0;
                    mmcArena.n = 0;
                    for(int i = 0; i<28; i++){
                        mmcArena.dot[i].act = false;
                    }
                    printf("MMC Close loop : stop\n");
                }
            }
        } else {
            if(in->mouseButtons[SDL_BUTTON_LEFT]){mousePos.x = in->mouseX; mousePos.y = in->mouseY;SDL_Delay(100);}
            if (in->key[SDLK_m] &&!in->key[SDLK_LCTRL] && !mmcOn){
                printf("Calibration de l'overlay, veuillez cliquer sur l'angle supérieur gauche de l'arène\n");
                mousePos.x = -1;
                mmcOn = true;
                overlayCalib = false;
                SDL_Delay(100);
            }
            if(in->key[SDLK_z] && spinDirection != 1){
                spinDirection = 1;
                printf("Coils positive\n");
                dataMAJ = true;
            } else if (in->key[SDLK_s] && spinDirection != -1){
                spinDirection = -1;
                printf("Coils negative\n");
                dataMAJ = true;
            } else if(!(in->key[SDLK_z] || in->key[SDLK_s] || spinDirection == 0)){
                spinDirection = 0;
                printf("Coils off\n");
                dataMAJ = true;
            }
            if(in->key[SDLK_q]){
                yawAngle=fmod(yawAngle+SensibilityAngle,2*PI);
                //printf("Yaw Angle : %d°\n",(int)(yawAngle /PI*180));
                dataMAJ = true;
            }
            if(in->key[SDLK_d]){
                yawAngle=fmod(yawAngle-SensibilityAngle,2*PI);
                if(yawAngle<0){yawAngle += 2*PI;}
                //printf("Yaw Angle : %d°\n",(int)(yawAngle /PI*180));
                dataMAJ = true;
            }
            if(in->key[SDLK_k] && pitchAngle < PI){
                pitchAngle += SensibilityRoll;
                if(pitchAngle > PI) pitchAngle = PI;
                printf("Pitch Angle : %d°\n",(int)(pitchAngle /PI*180));
                dataMAJ = true;
            }
            if(in->key[SDLK_j] && pitchAngle != 0){
                pitchAngle = 0;
                printf("Pitch Angle : 0°\n");
                dataMAJ = true;
            }
            if(in->key[SDLK_h] && pitchAngle > -PI){
                pitchAngle -= SensibilityRoll;
                if(pitchAngle < -PI) pitchAngle = -PI;
                printf("Pitch Angle : %d°\n",(int)(pitchAngle /PI*180));
                dataMAJ = true;
            }
            if(in->key[SDLK_f]){
                spinFrequency+=SensibilityFrequency;
                printf("Frequency : %f\n",spinFrequency);
            }
            if(in->key[SDLK_g]){
                spinFrequency-=SensibilityFrequency;
                printf("Frequency : %f\n",spinFrequency);
            }
            if(in->key[SDLK_c] && trackingOn){
                clCalibration = true;
            }
            if(in->key[SDLK_KP_4]) xGradient = 1; else if(in->key[SDLK_KP_6]) xGradient = -1; else xGradient = 0;
            if(in->key[SDLK_KP_8]) yGradient = 1; else if(in->key[SDLK_KP_2]) yGradient = -1; else yGradient = 0;
        }
    } else {
        if((in->key[SDLK_c] && in->key[SDLK_LCTRL]) || !trackingOn){
            clCalibration = false;
        }
    }
}

void updatEvents (Input *in){
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type) {
        case SDL_KEYDOWN:
            in->key[event.key.keysym.sym]=1;
            break;
        case SDL_KEYUP:
            in->key[event.key.keysym.sym]=0;
            break;
        case SDL_MOUSEMOTION:
            in->mouseX=event.motion.x;
            in->mouseY=event.motion.y;
            in->mouseRelX=event.motion.xrel;
            in->mouseRelY=event.motion.yrel;
            break;
        case SDL_MOUSEBUTTONDOWN:
            in->mouseButtons[event.button.button]=1;
            break;
        case SDL_MOUSEBUTTONUP:
            in->mouseButtons[event.button.button]=0;
            break;
        case SDL_QUIT:
            in->quit=true;
            break;
        }
    }
}
