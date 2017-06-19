#ifndef _YOLO_PID_H_
#define _YOLO_PID_H_

typedef struct
{
    double Kp ;             // proportional gain
    double Ki ;             // integral gain
    double Kd ;             // derivative gain

    double Error;           // error in the setpoint
    long   ErrorTime ;      // time of the error in the setpoint
    double BaseSpeed ;      // base speed of the robot used to compute magnigfy the
    double LastError;       // previous error in the setpoint,
    long   LastErrorTime;   // previous time of the previous error in the setpoint

    double Integral;        // computed integral;
    double Derivative;      // computed derivative
    double SpeedFudgeFactor;// fudge factor correction used in conjuction with the 
} TPID;

class PID
{
public:
    PID();
    ~PID();

    long getTimeMs();
    void initPID(double Kpi, double Kii, double Kdi, double baseSpeedi, double speedFudgeFactori);
    void setError(double errorIn);
    double runPID();

private:
    TPID    _pid;
};

#endif
