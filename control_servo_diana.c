
#include <stdio.h>
#include <phidget21.h>
#include <time.h>
#include <math.h>

int flag_motion=0;

double Get_Time()
/*----------------------------------
get the current time in seconds
------------------------------------*/
{
  double temps;
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  temps=now.tv_sec+1.e-9*now.tv_nsec;
  return temps;
}
double Get_Angle_From_Pos(double pos)
{
  return (-187.9281908 + 1.585891906*pos);
}
double Get_Pos_From_Angle(double angle)
{
  return (118.5 + 0.63056*angle);
}
int CCONV AttachHandler(CPhidgetHandle ADVSERVO, void *userptr)
{
  printf("phidget attache\n");
  return 0;
}

int CCONV DetachHandler(CPhidgetHandle ADVSERVO, void *userptr)
{
  printf("attention, phidget detache\n");
  return 0;
}
int CCONV ErrorHandler(CPhidgetHandle ADVSERVO, void *userptr, int ErrorCode, const char *Description)
{
  printf("Error handled. %d - %s\n", ErrorCode, Description);
  return 0;
}
int CCONV PositionChangeHandler(CPhidgetAdvancedServoHandle ADVSERVO, void *usrptr, int Index, double Value)
{
  if(flag_motion)
    printf("angle: %f\n",Get_Angle_From_Pos(Value));
  return 0;
}


int servo_simple()
{
  int result;
  double curr_pos;
  const char *err;
  double minAccel, maxVel;
  double angle, pas,temps;

  //Declare an advanced servo handle
  CPhidgetAdvancedServoHandle servo = 0;

  //create the advanced servo object
  CPhidgetAdvancedServo_create(&servo);

  //Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
  CPhidget_set_OnAttach_Handler((CPhidgetHandle)servo, AttachHandler, NULL);
  CPhidget_set_OnDetach_Handler((CPhidgetHandle)servo, DetachHandler, NULL);
  CPhidget_set_OnError_Handler((CPhidgetHandle)servo, ErrorHandler, NULL);

  //Registers a callback that will run when the motor position is changed.
  //Requires the handle for the Phidget, the function that will be called, 
  //and an arbitrary pointer that will be supplied to the callback 
  //function (may be NULL).
  CPhidgetAdvancedServo_set_OnPositionChange_Handler(servo, 
						     PositionChangeHandler, 
						     NULL);

  //open the device for connections
  CPhidget_open((CPhidgetHandle)servo, -1);

  // wait for the servo to be attached
  printf("Waiting for Phidget to be attached....");
  if((result = CPhidget_waitForAttachment((CPhidgetHandle)servo, 10000)))
    {
      CPhidget_getErrorDescription(result, &err);
      printf("probleme de connection au phidget\n");
      printf("verifier la connection USB\n");
      return 0;
    }

  //We assume servo motor is attached to index 0

  //Set up some initial acceleration and velocity values
  CPhidgetAdvancedServo_getAccelerationMin(servo, 0, &minAccel);
  CPhidgetAdvancedServo_setAcceleration(servo, 0, minAccel*2);
  CPhidgetAdvancedServo_getVelocityMax(servo, 0, &maxVel);
  CPhidgetAdvancedServo_setVelocityLimit(servo, 0, maxVel/2);

  CPhidgetAdvancedServo_setEngaged(servo, 0, 1);
  temps=Get_Time();
  while(Get_Time()-temps<0.5);
    
  while(1)
    {
      //servo ne bouge pas sinon on attend la fin du mouvement
      if(flag_motion==0)
	{
	  CPhidgetAdvancedServo_getPosition(servo, 0, &curr_pos);
	  printf("\nangle actuel: %f degre\n",Get_Angle_From_Pos(curr_pos));
	  printf(" quel position angulaire en degre souhaitez vous ");
	  printf("(200 pour sortir)?\n");
	  fflush(stdout);
	  scanf("%lf", &angle);
	}
      else
	{
	  CPhidgetAdvancedServo_getPosition(servo, 0, &curr_pos);
	  if(fabs(angle-Get_Angle_From_Pos(curr_pos))>0.1)
	    {
	  //	  flag_motion=0;
	      temps=Get_Time();
	      while(Get_Time()-temps>0.3);
	      CPhidgetAdvancedServo_getPosition(servo, 0, &curr_pos);
	      continue;
	    }
	  temps=Get_Time();
	  while(Get_Time()-temps>0.5);
	  flag_motion=0;
	  continue;
	}
      if(angle==200)goto erstop;
      //pas=118.5+(angle*0.63056);
      //il y a 0.6306 Pas par degrés et l'amplitude du servo varie entre 
      //57 et 199, le milieu, angle 0, est à 118.5        
      pas=Get_Pos_From_Angle(angle);

      if(pas<176 && pas>61)
        {
	  CPhidgetAdvancedServo_setPosition (servo, 0, pas);
	  temps=Get_Time();
	  flag_motion=1;
        }
      else
	{
	  printf("position est en dehors des limites du servo (-90 90)\n");
        }
    }
 erstop:
  printf("Closing...\n");
  CPhidgetAdvancedServo_setEngaged(servo, 0, 0);
	  temps=Get_Time();
	  while(Get_Time()-temps>0.3);
  CPhidget_close((CPhidgetHandle)servo);
  CPhidget_delete((CPhidgetHandle)servo);
  return 0;
}

int main(int argc, char* argv[])
{
  servo_simple();
  return 0;
}

