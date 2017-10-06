#include "coils.h"

#include <math.h>
#include "parameters.h"
#include "pid.h"
#include "826api.h"

Coils::Coils(QObject* CoilsPU):
    QThread(CoilsPU)
{
    this->coilsUnivers = CoilsPU;
    board = 0;
    counter = 0;
    tsample = 33;
    errcode = S826_ERR_OK;
    tCalib_start = 0;

    maxCoil = 16384 * 1;

    xHP = 0;    // Ampli 1
    yHP = 1;    // Ampli 2
    zHP = 2;    // Ampli 3
    xHP2 = 3;   // Ampli 4

    xGP1 = 4;   // Ampli 5
    xGP2 = 5;   // Ampli 6
    yGP1 = 6;   // Ampli 7
    yGP2 = 7;   // Ampli 8

    xOut = 0;
    yOut = 0;
    zOut = 0;

    xPhOut = 0;
    yPhOut = 0;
    zPhOut = 0;

    contX.setP(pVal);
    contX.setI(iVal);
    contX.setD(dVal);

    contY.setP(pVal);
    contY.setI(iVal);
    contY.setD(dVal);
}


Coils::~Coils(){
    printf("\nFermeture Control Bobine...");
    if(!PeriodicTimerStop(board,counter)) checkError();
    if((errcode = S826_DacDataWrite(board,xHP,32768,0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,xHP2,32768,0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,yHP,32768,0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,zHP,32768,0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,xGP1,32768, 0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,xGP2,32768, 0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,yGP1,32768, 0)) != S826_ERR_OK) checkError();
    if((errcode = S826_DacDataWrite(board,yGP2,32768, 0)) != S826_ERR_OK) checkError();
    S826_SystemClose();
}

void Coils::close(){
    tRun = false;
}

bool Coils ::PeriodicTimerStart(uint board, uint counter, uint period)
{
    if((errcode = S826_CounterStateWrite(board, counter, 0)) != S826_ERR_OK )
        return false; // halt channel if it's running
    if((errcode = S826_CounterModeWrite(board, counter, TMR_MODE)) != S826_ERR_OK)
        return false; // configure counter as periodic timer
    if((errcode = S826_CounterSnapshotConfigWrite(board, counter, 4, S826_BITWRITE)) != S826_ERR_OK)
        return false; // capture snapshots at zero counts
    if((errcode = S826_CounterPreloadWrite(board, counter, 0, period)) != S826_ERR_OK)
        return false; // timer period in microseconds
    if((errcode = S826_CounterStateWrite(board, counter, 1)) != S826_ERR_OK)     // start timer
        return false;
    return true;
}

bool Coils::PeriodicTimerWait(uint board, uint counter, uint *timestamp)
{
    uint counts, tstamp, reason;    // counter snapshot
    if((errcode = S826_CounterSnapshotRead(board, counter, &counts, &tstamp, &reason, S826_WAIT_INFINITE)) != S826_ERR_OK) // wait for timer snapshot
        return false;
    if (timestamp != NULL)
        *timestamp = tstamp;
    return true;
}

bool Coils::PeriodicTimerStop(uint board, uint counter)
{
    return ((errcode = S826_CounterStateWrite(board, counter, 0)) != S826_ERR_OK);
}

bool Coils::s826Init(){
    printf("\nInitialisation carte s826");
    int boardflag = S826_SystemOpen();
    if((errcode = boardflag) <0)
        return false;
    else if (errcode == 0)
        printf("\nPas de carte s826 détecté...");
    for(unsigned int i = 0; i < 8; i++){
        printf("\nInitialisation DAC n°%d",i);
        if((errcode = S826_DacRangeWrite(board,i, S826_DAC_SPAN_10_10, 0)) != S826_ERR_OK)
            return false;
    }
    printf("\nInitialisation des timer");
    if (!PeriodicTimerStart(board,counter,tsample))
        return false;
    if(!PeriodicTimerWait(board,counter,&tstamp))
        return false;
    if(!PeriodicTimerStart(board,1,tsample))
        tbegin = tstamp;
    tpid_old = tstamp;
    printf("\n>Ok\n");
    return true;

}

bool Coils::coilsMAJ(bool MAJ){
    if (MAJ){
        yOut = y1OutTunning * cos(yawAngle) * abs(spinDirection);
        xOut = x1OutTunning * sin(yawAngle) * abs(spinDirection);

        zPhOut = z1OutTunning * cos(pitchAngle-PI/2) * spinDirection;
        yPhOut = y1OutTunning *  sin(yawAngle) * sin(pitchAngle-PI/2) * spinDirection;
        xPhOut = x1OutTunning * -cos(yawAngle) * sin(pitchAngle-PI/2) * spinDirection;

        dataMAJ = false;
    }

    int xVal = 32768 + maxCoil * (x1OffSet + xOut * cos(2*PI*spinFrequency*runtime) + xPhOut * sin(2*PI*spinFrequency*runtime));
    int xVal2 = 32768 - maxCoil * (x1OffSet + xOut * cos(2*PI*spinFrequency*runtime) + xPhOut * sin(2*PI*spinFrequency*runtime));
    int yVal = 32768 + maxCoil * (y1OffSet + yOut * cos(2*PI*spinFrequency*runtime) + yPhOut * sin(2*PI*spinFrequency*runtime));
    int zVal = 32768 + maxCoil * (z1OffSet + zOut * cos(2*PI*spinFrequency*runtime) + zPhOut * sin(2*PI*spinFrequency*runtime));

    if((errcode = S826_DacDataWrite(board,xHP,xVal,0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,xHP2,xVal2,0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,yHP,yVal,0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,zHP,zVal,0)) != S826_ERR_OK) return false;

    if((errcode = S826_DacDataWrite(board,xGP1,32768 + std::min(xGradient,0) * 32767, 0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,xGP2,32768 + std::max(xGradient,0) * 32767, 0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,yGP1,32768 + std::min(yGradient,0) * 32767, 0)) != S826_ERR_OK) return false;
    if((errcode = S826_DacDataWrite(board,yGP2,32768 + std::max(yGradient,0) * 32767, 0)) != S826_ERR_OK) return false;

    //S826_DacDataWrite(board, xHP2, 64768,0);
    return true;
}

void Coils::PID(SDL_Point pos){

    double motorX, motorY;
    motorX = contX.next_value((double)pos.x,(double)robotPos.x,runtime);
    motorY = contY.next_value((double)pos.y,(double)robotPos.y,runtime);

    spinDirection = 1;
    if (- sqrt(pow(motorX,2) + pow(motorY,2))/100 > -0.4){
        pitchAngle = - sqrt(pow(motorX,2) + pow(motorY,2))/100;
    } else {
        pitchAngle = -0.4;
    }
    yawAngle = atan2((float)-(motorY),(float)(motorX)) - rollBias;

    dataMAJ = true;
}

void Coils::PIDrefrech(){
    contX.refresh();
    contY.refresh();
}

SDL_Point Coils::linearCommand(SDL_Point com, SDL_Point orr, double T, double Speed){
    SDL_Point CCom;
    double pSpeed = Speed*mmcArena.ratioPixMM;
    double dX = com.x-orr.x;
    double dY = com.y-orr.y;
    double TT = sqrt(dX*dX+dY*dY)/pSpeed;
    if(T>TT)
    {
        CCom.x = com.x;
        CCom.y = com.y;
    }
    else
    {
        CCom.x=dX*T/TT+orr.x;
        CCom.y=dY*T/TT+orr.y;
    }
    return CCom;
}

void Coils::checkError(){
    printf("\n---\nErreur controle bobine : ");
    switch (errcode)
    {
    case S826_ERR_BOARD:        printf("Illegal board number"); break;
    case S826_ERR_VALUE:        printf("Illegal argument"); break;
    case S826_ERR_NOTREADY:     printf("Device not ready or timeout"); break;
    case S826_ERR_CANCELLED:    printf("Wait cancelled"); break;
    case S826_ERR_DRIVER:       printf("Driver call failed"); break;
    case S826_ERR_MISSEDTRIG:   printf("Missed adc trigger"); break;
    case S826_ERR_DUPADDR:      printf("Two boards have same number"); break;
    case S826_ERR_BOARDCLOSED:  printf("Board not open"); break;
    case S826_ERR_CREATEMUTEX:  printf("Can't create mutex"); break;
    case S826_ERR_MEMORYMAP:    printf("Can't map board"); break;
    case S826_ERR_MALLOC:       printf("Can't allocate memory");break;
    case S826_ERR_FIFOOVERFLOW: printf("Overflow");break;
    default:                    printf("Unknown error"); break;
    }
    printf("\n---\n");
}

void Coils::run(){
    tRun = s826Init();
    if(!tRun) checkError();
    while(tRun){
        runtime = (double)(tstamp - tbegin)/1000000;
        if(mmcOn){
            if (clStart && mmcArena.n > 1){
                if(sqrt(pow(robotPos.x-mmcArena.dot[mmcArena.order[1]].pos.x,2)+pow(robotPos.y-mmcArena.dot[mmcArena.order[1]].pos.y,2)) < mmcArena.radius){
                    for(int i = 0; i< mmcArena.n; i++){
                        mmcArena.order[i] = mmcArena.order[i+1];
                    }
                    mmcArena.n--;
                    tpid_old = runtime;
                    PIDrefrech();
                }
                tpid = runtime - tpid_old;
                comPos = linearCommand(mmcArena.dot[mmcArena.order[1]].pos, mmcArena.dot[mmcArena.order[0]].pos,tpid,500);
                PID(comPos);
            } else if (overlayCalib){
                tpid_old = runtime;
                PIDrefrech();
                PID(mmcArena.dot[mmcArena.order[0]].pos);
            }
        }
        if (clCalibration){
            if(tCalib_start == 0){
                printf("Démarage de la calibration, ne touchez à rien svp...\n");
                tCalib_start = runtime;

                rollBias = 0;
                spinDirection = 1;
                dataMAJ = true;
                comPos=robotPos;
                printf("%d x %d\n", comPos.x, comPos.y);
            }
            tCalib = runtime-tCalib_start;
            if (tCalib >= 1){
                spinDirection = 0;
                dataMAJ = true;
                rollBias = atan2((float)-(robotPos.y-comPos.y),(float)(robotPos.x-comPos.x)) - yawAngle;
                printf("rollBias : %f°\n",rollBias/PI*180);
                tCalib_start = 0;
                clCalibration = false;
                comPos.x = 0;
                comPos.y = 0;
                printf("Fin de la calibration...\n");
            }
        }
        try{
            coilsMAJ(dataMAJ);
        } catch(...){
            try{coilsMAJ(dataMAJ);
            } catch(...){
                checkError();
            }
        }
        try{
            PeriodicTimerWait(board,counter,&tstamp);
        } catch(...){
            try{
                PeriodicTimerWait(board,counter,&tstamp);
            } catch(...){
                checkError();}
        }
    }
}
