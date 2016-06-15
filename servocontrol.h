
#include <Servo.h>

class ServoControl
{
  int mPin;
  Servo mServo;
  
public:
  ServoControl(int pin)
  : mPin(pin)
  {
  }

  void setup()
  {
    mServo.attach(mPin);
    nextBall();
  }

  void release()
  {
    mServo.write(30);
  }

  void nextBall()
  {
    mServo.write(0);
  }

  void prepareRelease()
  {
    mServo.write(17);
  }
};

