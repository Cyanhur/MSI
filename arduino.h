#ifndef ARDUINO_H
#define ARDUINO_H

typedef enum {
    X1OT,   //X1 out tunning
    X2OT,   //X2 "
    Y1OT,   //Y1 "
    Y2OT,   //Y2 "
    Z1OT,   //Z1 "
    Z2OT,   //Z2 "
    X1OS,   //X1 off set
    X2OS,   //X2 "
    Y1OS,   //Y1 "
    Y2OS,   //Y2 "
    Z1OS,   //Z1 "
    Z2OS,   //Z2 "
    YA,     //yaw angle
    PA,     //pitch angle
    SF,     //spin frequency
    LI,     //light intensity
    START,  //Start Arduino
    STOP,   //Stop Arduino
    HM
} ArduinoCode;

bool initArduino();
bool sendToArduino(ArduinoCode code);
void closeArduino();

#endif // ARDUINO_H
