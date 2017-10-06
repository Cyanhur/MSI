#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <SDL2/SDL.h>

struct mmcDot{
    SDL_Point pos;
    bool act;
};

struct arena {
    SDL_Rect arenaBorder;
    float ratioPixMM;
    float radius;
    mmcDot dot[28];
    int order[28];
    int n;
};

//----------------CONSTANTS-------------

const int VC_HEIGHT = 480;                 // height of the captured video
const int VC_WIDTH = 640;                 // width "
const double VC_FREQ = 24.0;               // frequency "
const int nPixelClockDefault = 9;          //
const int SC_HEIGHT = 768;                 // height of the screen
const int SC_WIDTH = 1024;                 // width "
const int LIMIT_ST_PITCH = 1;              // limite pitch angle for close loop in Spintop motion
const float PI = 3.14159;

//----------------PARAMETERS--------------
extern char conf_file[64];                  // Name of the configuration file

// Camera parameters

extern unsigned char* pOld;
extern unsigned char* pNew;
extern int bytesPP;

// Coils parameters

extern float x1OutTunning;                  // x1 coil, this is the tuning made by measuring the ratio volt from the bc to magnetic field in the center of the coil with a gaussmeter
extern float x2OutTunning;                  // x2 coil,  "
extern float y1OutTunning;                  // y1 coil,  "
extern float y2OutTunning;                  // y2 coil,  "
extern float z1OutTunning;                  // z1 coil,  "
extern float z2OutTunning;                  // z2 coil,  "
extern float x1OffSet;                      //
extern float x2OffSet;                      //
extern float y1OffSet;                      //
extern float y2OffSet;                      //
extern float z1OffSet;                      //
extern float z2OffSet;                      //
extern float globalPower;                   // between 0 and 10, for inverted setup 0.25=8mT. Increase until 12 should be possible with correct power supply.
extern float yawAngle;                      // Yaw angle of the magnetic field
extern float pitchAngle;                    // Pitch angle of the magnetic field (between -pi/2 and pi/2)
extern float spinFrequency;                 // frequency of the spin (Hz)
extern int spinDirection;                   // direction of the spin (-1, 0 or 1)

extern int xGradient;
extern int yGradient;

extern bool dataMAJ;

//tracking parameters

extern arena mmcArena;
extern SDL_Point mousePos;
extern SDL_Point robotPos;
extern SDL_Point comPos;
extern bool trackingOn;
extern bool trackingStarted;
extern bool mmcOn;
extern bool overlayCalib;

//close loop parameters

extern bool clCalibration;
extern bool clStart;
extern double pVal;
extern double iVal;
extern double dVal;
extern float rollBias;

//if the following Parameter are defined in conf file, they will be overwritten
extern float presicion;
extern float SensibilityFrequency;         // here you can tune of fast the frequency move by pressing 'g' and 't' (Arbitrary unity)
extern float SensibilityAngle;             // here you can tune of fast the angle move by pressing 'q' and 'd' (Arbitrary unity)
extern float SensibilityRoll;              // here ohow fast the roll angle move
extern float Global_power_tuning;
extern float Power_up;                     //  shortkey for power use uijn cut odd determination
extern float Power_down;
extern float boost;                        // how much the sensibillityRoll is multiply when boost is press
extern float slow;                         // how much the sensibillityRoll is multiply when boost is press
extern float slow_tun;                     // sensibillity for the tuning of the slow key with '0' and '.'
extern float variator;                     // bosst buffer to switch between 1 (no boost) and boost value.
extern float SensibilityYaw;               // here how fast the yaw angle move in precise rolling mode
extern float drop_test_offset;             // power during drop test to block the robot with constant field
extern float Sensibilityoffset;            //Sensibillituy of the tuning of the coil power

#endif // PARAMETERS_H
