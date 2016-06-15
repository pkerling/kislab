/**
 * Inhibit releasing the ball when the turntable speed varies too much
 * 
 * \ref lightSpeedCallback and \ref hallSpeedCallback must be called whenever
 * a new speed measurement by the respective sensor is ready.
 * 
 * The hall sensor callback is used for determining how long the inhibition lasts
 * because the actual release time calculations are tied to the hall sensor.
 * The light sensor callback is used for determining when to inhibit because the
 * hall sensor only has state transitions two times per round, which is not enough
 * to reliably determine changes when the turntable speed is low.
 * After a change was detected, inhibition lasts for two (hall sensor) rounds. This 
 * is to ensure that the hall sensor measurement is stable enough for further calculations.
 * If during inhibition the speed changes again, the inhibition is again reset
 * to two rounds.
 */
class Inhibitor
{
  /// Maximum turn time difference between measurements to tolerate in µs
  const unsigned short MAXIMUM_TURN_TIME_DIFFERENCE = 10000;

  /// Previous turn time to compare new one against
  unsigned long mLastTurnTime = 0;
  
  /** 
   * Number of remaining full turntable rounds ball release is inhibited for
   * 
   * Ball release is not inhibited if this is zero (the default).
   */
  unsigned short mInhibitRounds = 0;
  
public:
  /**
   * Detect changes in the turntable speed via the light sensor
   */
  void lightSpeedCallback(unsigned long turnTime)
  {
    // Cast to (signed) long as result could be negative
    if (abs(static_cast<long> (mLastTurnTime) - static_cast<long> (turnTime)) > MAXIMUM_TURN_TIME_DIFFERENCE) {
      mInhibitRounds = 2;
    }
    mLastTurnTime = turnTime;
  }

  /**
   * Decrease remaining inhibited rounds when a round completes
   */
  void hallSpeedCallback(unsigned long)
  {
    if (mInhibitRounds > 0) {
      mInhibitRounds--;
    }
  }

  /**
   * Get whether ball release is currently inhibited
   */
  bool isInhibited() const
  {
    return (mInhibitRounds != 0);
  }
};

