#include "pid.h"

void Controller_gain::setP(double in){
    P = in;
}

void Controller_gain::setI(double in){
    I = in;
}

void Controller_gain::setD(double in){
    D = in;
}

double Controller::next_value(double command, double feedback, double time){
    double Dt = time-time_old;
    if (fresh){
        epsilon = command-feedback;
        out = P * epsilon;
        fresh = false;
        epsilon_old = epsilon;
        time_old = time;
        return out;
    } else {
        epsilon = command-feedback;
        epsilon_sum = epsilon_sum + (epsilon + epsilon_old)/(2.0) * (double)Dt * 0.001;
        out = P * epsilon + I * epsilon_sum + D * (epsilon - epsilon_old) / ((double)Dt * 0.001);
        epsilon_old = epsilon;
        time_old = time;
        return out;
    }
}

void Controller::refresh(){
    fresh = true;
    epsilon_old = 0.0;
    epsilon_sum = 0.0;
}

