#include <time.h>
#include <math.h>
#include <inttypes.h>
#include "PID.h"


PID::PID()
{
}

PID::~PID()
{
}

long
PID::getTimeMs()
{
    long ms;	    // ms
    time_t s;	    // sec
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6);

    return ((intmax_t)s*1000 + ms);
}

void
PID::initPID(double Kpi, double Kii, double Kdi, double baseSpeedi, double speedFudgeFactori)
{
    //_pid.ErrorTime = getTimeMs();
    _pid.Kp = Kpi;
    _pid.Ki = Kii;
    _pid.Kd = Kdi;

    //_pid.Error = 0;
    _pid.BaseSpeed = baseSpeedi;
    _pid.SpeedFudgeFactor = speedFudgeFactori;
    _pid.LastError = 0.0;
    _pid.Integral = 0.0;
    //_pid.Derivative = 0.0;
}

void
PID::setError(double errorIn)
{
    //_pid.Error = (_pid.BaseSpeed / _pid.SpeedFudgeFactor) * errorIn;
    //_pid.LastErrorTime =  _pid.ErrorTime;
    //_pid.ErrorTime = getTimeMs();
}

double
PID::runPID()
{
    /*
    long iterationTime = (long) (_pid.ErrorTime - _pid.LastErrorTime);

    _pid.Integral = (_pid.Error / iterationTime) + _pid.Integral;

    _pid.Derivative = (_pid.Error - _pid.LastError) / iterationTime;

    double correction = _pid.Kp * _pid.Error + _pid.Ki * _pid.Integral + _pid.Kd * _pid.Derivative;

    _pid.LastError = _pid.Error;

    return(correction);
    */
    return 0.0;
}
