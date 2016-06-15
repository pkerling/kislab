class Inhibitor
{
  const unsigned short MAXIMUM_TURN_TIME_DIFFERENCE = 10000;
  
  unsigned long mLastTurnTime = 0;
  unsigned short mInhibitRounds = 0;
  
public:
  void lightSpeedCallback(unsigned long turnTime)
  {
    if (abs(static_cast<long> (mLastTurnTime) - static_cast<long> (turnTime)) > MAXIMUM_TURN_TIME_DIFFERENCE) {
      mInhibitRounds = 2;
    }
    mLastTurnTime = turnTime;
  }

  void hallSpeedCallback(unsigned long)
  {
    if (mInhibitRounds > 0) {
      mInhibitRounds--;
    }
  }

  bool isInhibited()
  {
    return (mInhibitRounds != 0);
  }
};

