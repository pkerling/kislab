class StateMachine
{
public:
  enum class State {
    BALL_FALL_THROUGH,
    BALL_FALLING_THROUGH,
    RESETTING,
    IDLE,
    WAIT_TIME,
    RELEASING,
  };

  typedef unsigned long (*TimeProviderType)(void);

private:
  State mState = State::BALL_FALL_THROUGH;
  const int mTriggerPin;
  unsigned long mWaitUntil = 0;
  unsigned long mSavedLastCrossingTime = 0;
  const unsigned long FALL_TIME_US = 391000 + 28125;
  ServoControl& mServoControl;
  TimeProviderType mTurnTimeProvider = nullptr;
  TimeProviderType mLastCrossingTimeProvider = nullptr;

  void setWaitFromNow(unsigned long us)
  {
    mWaitUntil = micros() + us;
  }

  bool isWaitDone()
  {
    return (micros() >= mWaitUntil);
  }

  unsigned long calculateNextTurnTime(unsigned long currentTurnTime)
  {
    double x = currentTurnTime;
    if (currentTurnTime < 2000000UL) {
      const double a2 = 1.28978803947511e-8;
      const double a1 = 0.9910487474;
      const double a0 = 2585.5449062138;
      auto ret = a2 * pow(x, 2) + a1 * x + a0;
      return ret;
    } else {
      const double a4 = 3.10525846921924e-21;
      const double a3 = -3.96812534603555e-14;
      const double a2 = 2.19266935692185e-7;
      const double a1 = 0.5422132849;
      const double a0 = 343733.782110879;
      auto ret = a4 * pow(x, 4) + a3 * pow(x, 3) + a2 * pow(x, 2) + a1 * x + a0;
      return ret;
    }
  }

  unsigned long releaseWaitTime()
  {
    auto now = micros();
    auto lastCrossing = mLastCrossingTimeProvider();
    auto timeInRound = now - lastCrossing;
    auto t1 = micros();

    debugprintln("--");
    debugprint("now: ");
    debugprint(now);
    debugprint(" last crossing: ");
    debugprint(lastCrossing);
    debugprint(" -> time in round: ");
    debugprintln(timeInRound);

    auto turnTime = mTurnTimeProvider();
    long fallTimeAcc = - timeInRound;
    auto nextTurnTime = turnTime;
    while (1) {
      debugprint("old time: ");
      debugprint(nextTurnTime);
      nextTurnTime = calculateNextTurnTime(nextTurnTime);
      debugprint(" next turn time: ");
      debugprint(nextTurnTime);
      fallTimeAcc += nextTurnTime;
      debugprint(" acc time: ");
      debugprintln(fallTimeAcc);
      if (fallTimeAcc >= FALL_TIME_US) {
        auto waitTime = fallTimeAcc - FALL_TIME_US;
        debugprint("-> waitTime: ");
        debugprintln(waitTime);
        auto t2 = micros();
        auto dif = t2 - t1;
        debugprint("calc time: ");
        debugprintln(dif);
        return waitTime;
      }
    }
    
    auto rounds = FALL_TIME_US / turnTime;
    auto timeInFirstRound = FALL_TIME_US - rounds * turnTime;
    auto waitTime = turnTime - timeInFirstRound;
    return waitTime;
  }

public:
  StateMachine(ServoControl& servoControl, int triggerPin)
  : mServoControl(servoControl), mTriggerPin(triggerPin)
  {}

  void setTurnTimeProvider(TimeProviderType timeProvider)
  {
    mTurnTimeProvider = timeProvider;
  }

  void setLastCrossingTimeProvider(TimeProviderType timeProvider)
  {
    mLastCrossingTimeProvider = timeProvider;
  }

  void setup()
  {
    pinMode(mTriggerPin, INPUT);
  }

  void advanceState()
  {
    /*Serial.println("old state: ");
    Serial.println(mState);*/
    switch (mState) {
      case State::BALL_FALL_THROUGH:
        mServoControl.nextBall();
        setWaitFromNow(400000);
        mState = State::BALL_FALLING_THROUGH;
      break;
      case State::BALL_FALLING_THROUGH:
        if (isWaitDone()) {
          mServoControl.prepareRelease();
          setWaitFromNow(200000);
          mState = State::RESETTING;
        }
      break;
      case State::RESETTING:
        if (isWaitDone()) {
          mState = State::IDLE;
        }
      break;
      case State::IDLE:
        if (triggerState()) {
          mState = State::WAIT_TIME;
          setWaitFromNow(releaseWaitTime());
        }
      break;
      case State::WAIT_TIME:
        if (isWaitDone()) {
          mServoControl.release();
          setWaitFromNow(200000);
          mState = State::RELEASING;
        }
      break;
      case State::RELEASING:
        if (isWaitDone()) {
          setWaitFromNow(200000);
          mState = State::BALL_FALL_THROUGH;
        }
      break;

/*      case INHIBITED:
        if (!isInhibited()) {
          mState = IDLE;
        }
      break;*/
    }
    /*Serial.println("new state: ");
    Serial.println(mState);*/
  }
  
  bool triggerState()
  {
    return digitalRead(mTriggerPin);
  }
  
  bool isInhibited()
  {
  }  

};

