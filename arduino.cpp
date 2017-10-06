#include <iostream>
#include <unistd.h>
#include<SDL2/SDL.h>

#include "arduino.h"
#include "parameters.h"

FILE* file;

bool initArduino()
{
    printf("\nOuverture du port ttyACM0 : ");
    if( NULL == (file = fopen("/dev/ttyACM0","wrb"))){
        printf("Echec !\nOuverture du port ttyACM1 : ");
        if(NULL == (file = fopen("/dev/ttyACM1","wrb")))
        {
            printf("Echec !\n");
            return false;
        }
    }
    printf("Succes\n\nEnvoi des donnees d'initialisation :\n");
    SDL_Delay(2000);
    setbuf(file, NULL);
    if (!sendToArduino(X1OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(X2OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Y1OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Y2OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Z1OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Z2OT))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(X1OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(X2OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Y1OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Y2OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Z1OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(Z2OS))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(PA))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(YA))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(SF))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(LI))
        return false;
    SDL_Delay(10);
    if (!sendToArduino(START))
        return false;
    return true;
}

void closeArduino(){
    sendToArduino(STOP);
    fclose(file);
}

bool sendToArduino(ArduinoCode code){
    int sData = 0;
    char buff[80];
    memset(buff, 0, sizeof(buff));
    fflush(file);
    std::cout<<"Arduino : ";
    switch (code) {
    case START :{
        printf("Arduino start !");
        fprintf(file,"%d",1);
        break;
    }
    case STOP :{
        printf("Arduino stop !");
        fprintf(file,"%d",0);
        break;
    }
    case X1OT :{ //X1OutTunning
        sData = x1OutTunning * 100;
        fprintf(file,"%d",8);
        printf("X1 Out Tunning = %d", sData);
        break;
    }
    case X2OT :{ //X2OutTunning
        sData = x2OutTunning * 100;
        fprintf(file,"%d",9);
        printf("X2 Out Tunning = %d", sData);
        break;
    }
    case Y1OT :{ //Y1OutTunning
        sData = y1OutTunning * 100;
        fprintf(file,"%d",10);
        printf("Y1 Out Tunning = %d", sData);
        break;
    }
    case Y2OT :{ //Y2OutTunning
        sData = y2OutTunning * 100;
        fprintf(file,"%d",11);
        printf("Y2 Out Tunning = %d", sData);
        break;
    }
    case Z1OT :{ //Z1OutTunning
        sData = z1OutTunning * 100;
        fprintf(file,"%d",12);
        printf("Z1 Out Tunning = %d", sData);
        break;
    }
    case Z2OT :{ //Z2OutTunning
        sData = z2OutTunning * 100;
        fprintf(file,"%d",13);
        printf("Z2 Out Tunning = %d", sData);
        break;
    }
    case X1OS :{ //x1OffSet
        fprintf(file,"%d",2);
        sData = x1OffSet * 1000;
        printf("X1 OffSet = %d", sData);
        break;
    }
    case X2OS :{ //x2OffSet
        fprintf(file,"%d",3);
        sData = x2OffSet * 1000;
        printf("X2 OffSet = %d", sData);
        break;
    }
    case Y1OS :{ //y1OffSet
        fprintf(file,"%d",4);
        sData = y1OffSet * 1000;
        printf("Y1 OffSet = %d", sData);
        break;
    }
    case Y2OS :{ //y2OffSet
        fprintf(file,"%d",5);
        sData = y2OffSet * 1000;
        printf("Y2 OffSet = %d", sData);
        break;
    }
    case Z1OS :{ //z1OffSet
        fprintf(file,"%d",6);
        sData = z1OffSet * 1000;
        printf("Z1 OffSet = %d", sData);
        break;
    }
    case Z2OS :{ //z2OffSet
        fprintf(file,"%d",7);
        sData = z2OffSet * 1000;
        printf("Z2 OffSet = %d", sData);
        break;
    }
    case PA :{ //pAngle
        sData = pitchAngle * 100;
        fprintf(file,"%d",14);
        printf("Pitch Angle = %d", sData);
        break;
    }
    case YA :{ //yAngle
        sData = yawAngle * 100;
        fprintf(file,"%d",15);
        printf("Yaw Angle = %d", sData);
        break;
    }
    case SF :{ //fSpin
        sData = spinFrequency;
        fprintf(file,"%d",16);
        printf("Frequency = %d", sData);
        break;
    }
    case LI:{
        sData = lightInt;
        fprintf(file,"%d",17);
        printf("Ligh Intensity = %d", sData);
        break;
    }
    case HM:{
        sData = spinDirection;
        fprintf(file,"%d",18);
        printf("Spinmod : %d", sData);
        break;
    }
    }
    if( 0 >= fprintf(file,",%d;",sData)){
        printf(" : Echec !\n" );
        return false;
    }
    printf(" : Succes\n");
    return true;
}
