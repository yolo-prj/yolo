//------------------------------------------------------------------------------------------------
// File: Servos.h
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Provides the ability to control the servos on the Robot wheels and camera
//------------------------------------------------------------------------------------------------
#ifndef ServosH
#define ServosH

#define SERVO_CENTER_OR_STOP 150

#define TRK_LINE_CAM_PAN     149 //157
#define TRK_LINE_CAM_TILT    211 //192


#define CAMERA_PAN  0
#define CAMERA_TILT 1
#define RIGHT_WHEEL 2
#define LEFT_WHEEL  3
#define NUM_SERVOS  4

#define PAN_CAMERA_MIN    50
#define PAN_CAMERA_MAX   250
#define TILT_CAMERA_MIN   80
#define TILT_CAMERA_MAX  220 
#define WHEEL_MAX        200
#define WHEEL_MIN        100

int  OpenServos(void);
void CloseServos(void);
int  SetServoPosition(unsigned int Servo,int &Position);
void SetWheelSpeed(int left,int right);
void ResetServos(void);
void SetCameraServosLineTrackMode(int &Pan,int &Tilt);
void SetCameraServosLineTrackMode(void);


#endif
//------------------------------------------------------------------------------------------------
//END of Include
//------------------------------------------------------------------------------------------------
