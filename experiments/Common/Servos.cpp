//------------------------------------------------------------------------------------------------
// File: Servos.cpp
// Project: LG Exec Ed Program
// Versions:
// 1.0 April 2017 - initial version
// Provides the ability to control the servos on the Robot wheels and camera
//----------------------------------------------------------------------------------------------
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "Servos.h"

#define LIMIT_MIN 0
#define LIMIT_MAX 1

static int ServoLimits[NUM_SERVOS][2]={{PAN_CAMERA_MIN,PAN_CAMERA_MAX},
                                       {TILT_CAMERA_MIN,TILT_CAMERA_MAX},
                                       {WHEEL_MIN,WHEEL_MAX},
                                       {WHEEL_MIN,WHEEL_MAX}
                                      };


static int ServoFd=-1;
//----------------------------------------------------------------
// OpenServos - Opens the Servo Devices return 0 on success and 
// -1 on failure
//-----------------------------------------------------------------
int OpenServos(void)
{
 
 if (ServoFd!=-1) return(0);
 ServoFd=open("/dev/servoblaster",O_RDWR);
 if (ServoFd<0)
   {
    return(-1);
   }
  sleep(1); // wait for the servo driver to stabilize 
  return(0);
}
//-----------------------------------------------------------------
// END OpenServos
//-----------------------------------------------------------------
//----------------------------------------------------------------
// CloseServos - Closes thhe Servo Devices
//-----------------------------------------------------------------
void CloseServos(void)
{
 
 if (ServoFd==-1) return;
 close(ServoFd);
 ServoFd=-1;
}
//-----------------------------------------------------------------
// END OpenServos
//-----------------------------------------------------------------

//----------------------------------------------------------------
// SetServoPosition - Set the servo to the specified position 
// returns 0 on success and  -1 on failure
///---------------------------------------------------------------
int SetServoPosition(unsigned int Servo,int &Position)
{
  char buf[256];
  size_t len;
  if (ServoFd==-1) return(-1); 

  if (Servo>=NUM_SERVOS) return(-1);
  
  if (Position>ServoLimits[Servo][LIMIT_MAX]) 
      Position=ServoLimits[Servo][LIMIT_MAX];
  else if  (Position<ServoLimits[Servo][LIMIT_MIN]) 
      Position=ServoLimits[Servo][LIMIT_MIN];

  sprintf(buf,"%d=%d\n",Servo,Position);
  len=strlen(buf);
  if (write(ServoFd,buf,len)!=len) return(-1);
  fsync(ServoFd);
  return(0);
}
//-----------------------------------------------------------------
// END OpenServos
//-----------------------------------------------------------------
//----------------------------------------------------------------
// ResetServos - Reset All Servpo to their center position or 
// stopped 
///---------------------------------------------------------------
void ResetServos(void)
{
 int Value=SERVO_CENTER_OR_STOP;
 SetServoPosition(RIGHT_WHEEL,Value);
 SetServoPosition(LEFT_WHEEL,Value);
 SetServoPosition(CAMERA_PAN,Value);
 SetServoPosition(CAMERA_TILT,Value);
}
//-----------------------------------------------------------------
// END ResetServos
//-----------------------------------------------------------------
//----------------------------------------------------------------
// SetWheelSpeed - Sets the left and right wheel speed of the
// wheel servos
///---------------------------------------------------------------
void SetWheelSpeed(int left,int right)
{
 left=left+SERVO_CENTER_OR_STOP;
 right=-right+SERVO_CENTER_OR_STOP;
 SetServoPosition(LEFT_WHEEL,left);
 SetServoPosition(RIGHT_WHEEL,right);		
}
//-----------------------------------------------------------------
// END SetWheelSpeed
//-----------------------------------------------------------------
//----------------------------------------------------------------
// SetCameraServosLineTrackMode - Puts the camera in the position
// to track the line and sets pan and tilt tp that position
///---------------------------------------------------------------
void SetCameraServosLineTrackMode(int &Pan,int &Tilt)
{
  Pan=TRK_LINE_CAM_PAN;
  Tilt=TRK_LINE_CAM_TILT;
  SetServoPosition(CAMERA_PAN,Pan);
  SetServoPosition(CAMERA_TILT,Tilt);
}
//-----------------------------------------------------------------
// END SetCameraServosLineTrackMode
//-----------------------------------------------------------------
//----------------------------------------------------------------
// SetCameraServosLineTrackMode - Puts the camera in the position
// to track the line
///---------------------------------------------------------------
void SetCameraServosLineTrackMode(void)
{
  int Pan=TRK_LINE_CAM_PAN;
  int Tilt=TRK_LINE_CAM_TILT;
  SetServoPosition(CAMERA_PAN,Pan);
  SetServoPosition(CAMERA_TILT,Tilt);
}
//-----------------------------------------------------------------
// END SetCameraServosLineTrackMode
//-----------------------------------------------------------------
//-----------------------------------------------------------------
// END of File
//-----------------------------------------------------------------
