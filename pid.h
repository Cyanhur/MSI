#ifndef PID_H
#define PID_H


class Controller_gain
{
public:

    void setP(double in);
    void setI(double in);
    void setD(double in);

protected:

    double P;
    double I;
    double D;

};

class Controller: public Controller_gain
{
public:

    double next_value(double command, double feedback, double time);
    void refresh();

private:

    bool fresh;
    double epsilon, epsilon_old, epsilon_sum;
    long time_old;
    double out;
};

#endif // PID_H
