#ifndef COILS_H
#define COILS_H

#include <QtGui>
#include <stdio.h>
#include <stdlib.h>

#include "826api.h"
#include "pid.h"
#include <SDL2/SDL.h>


#define TMR_MODE  (S826_CM_K_1MHZ | S826_CM_UD_REVERSE | S826_CM_PX_ZERO | S826_CM_PX_START | S826_CM_OM_NOTZERO)

class Coils : public QThread
{
    Q_OBJECT
public:

    Coils(QObject* CoilsPU = NULL);
    ~Coils();
    void run();
    void close();

private:

    bool PeriodicTimerStart(uint board, uint counter, uint period);
    bool PeriodicTimerWait(uint bord, uint counter, uint *timestamp);
    bool PeriodicTimerStop(uint boerd, uint counter);

    bool s826Init();
    bool coilsMAJ(bool MAJ);
    void PID(SDL_Point pos);
    void PIDrefrech();
    SDL_Point linearCommand(SDL_Point com, SDL_Point orr, double T, double Speed);

    void checkError();

    QObject* coilsUnivers;

    uint board, counter;
    uint tsample, tstamp, tbegin;
    double runtime;
    double tpid, tpid_old;
    double tCalib, tCalib_start;

    Controller contX;
    Controller contY;

    int errcode;
    int maxCoil;
    float xOut, yOut, zOut;
    float xPhOut, yPhOut, zPhOut;
    int xHP, xHP2, yHP, zHP;
    int xGP1, xGP2, yGP1, yGP2;

    bool tRun;
};

#endif // COILS_H
