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
  typedef bool (*StateProviderType)(void);

private:
  State mState = State::BALL_FALL_THROUGH;
  
  unsigned long mWaitUntil = 0;
  ServoControl& mServoControl;

  TimeProviderType mReleaseTimeCalculator = nullptr;
  StateProviderType mTriggerProvider = nullptr;
  StateProviderType mInhibitionProvider = nullptr;

  void setWaitFromNow(unsigned long us)
  {
    mWaitUntil = micros() + us;
  }

  bool isWaitDone()
  {
    return (micros() >= mWaitUntil);
  }

public:
  StateMachine(ServoControl& servoControl)
  : mServoControl(servoControl)
  {}

  void setReleaseTimeCalculator(TimeProviderType releaseTimeCalculator)
  {
    mReleaseTimeCalculator = releaseTimeCalculator;
  }

  void setTriggerProvider(StateProviderType triggerProvider)
  {
    mTriggerProvider = triggerProvider;
  }

  void setInhibitionProvider(StateProviderType inhibitionProvider)
  {
    mInhibitionProvider = inhibitionProvider;
  }

  void advanceState()
  {
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
        if (mTriggerProvider()) {
          mState = State::WAIT_TIME;
          setWaitFromNow(mReleaseTimeCalculator());
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
  }
};

