#include "parameters.h"

// parameter can be adjust here.
// note: for close loop control : the round value of the frequency is taken to smooth the output value and get lees noise. this is made in fuction.cpp/ CLoseLoopControl. Improve a nice smooth could be great : maybe make a PDFB to avoid the derivative kick in derivative gaain and increase derivative gain can do the trick. right now derivative gain is not stable due to noise and kick
//

// Camera parameters

unsigned char* pOld = NULL;
unsigned char* pNew = NULL;
int bytesPP = 3;

// Coils parameters
// Last calibration : 2017/08/16
float x1OutTunning = 1;                 // x1 coil, this is the tuning made by measuring the ratio volt from the bc to magnetic field in the center of the coil with a teslameter
float x2OutTunning = 1;                 // x2 coil,  "
float y1OutTunning = 0.948;             // y1 coil,  "
float y2OutTunning = 1;                 // y2 coil,  "
float z1OutTunning = -0.982;            // z1 coil,  "
float z2OutTunning = 1;                 // z2 coil,  "
float x1OffSet = 0.0304;                //
float x2OffSet = 0;                     //
float y1OffSet = 0;                     //
float y2OffSet = 0;                     //
float z1OffSet = 0;                     //
float z2OffSet = 0;                     //
float globalPower = 1;                  // between 0 and 10, for inverted setup 0.25=8mT. Increase until 12 should be possible with correct power supply.
float yawAngle = 0;                     // Yaw angle of the magnetic field
float pitchAngle = 0;                   // Pitch angle of the magnetic field (between -pi/2 and pi/2)
float spinFrequency = 50;               // frequency of the spin (Hz)
int spinDirection = 0;                  // direction of the spin (-1, 0 or 1)

int xGradient = 0;
int yGradient = 0;

bool dataMAJ = false;

//tracking parameters

arena mmcArena = {{0,0,0,0},0,0,{0,0,0},{0},0};
SDL_Point mousePos = {0,0};
SDL_Point robotPos = {0,0};
SDL_Point comPos = {0,0};
bool trackingOn = false;
bool trackingStarted = false;
bool mmcOn = false;
bool overlayCalib = false;

//close loop parameters

bool clCalibration = false;
bool clStart = false;
double pVal = 1;
double iVal = 0;
double dVal = 0;
float rollBias = 0;

//others...
float presicion = 0.95;
float SensibilityFrequency = 1;         // here you can tune of fast the frequency move by pressing 'g' and 't' (Arbitrary unity)
float SensibilityAngle = PI/180;             // here you can tune of fast the angle move by pressing 'q' and 'd' (Arbitrary unity)
float SensibilityRoll = PI/180;              // here ohow fast the roll angle move
float Global_power_tuning = 0.002;
float Power_up = 2;                     //  shortkey for power use uijn cut odd determination
float Power_down = 1;
float boost = 10;                        // how much the sensibillityRoll is multiply when boost is press
float slow = 0.1;                         // how much the sensibillityRoll is multiply when boost is press
float slow_tun = 0.01;                     // sensibillity for the tuning of the slow key with '0' and '.'
float variator = 1;                     // bosst buffer to switch between 1 (no boost) and boost value.
float SensibilityYaw = 0.3;               // here how fast the yaw angle move in precise rolling mode
float drop_test_offset = 0;             // power during drop test to block the robot with constant field
float Sensibilityoffset = 0.2;            //Sensibillituy of the tuning of the coil power
