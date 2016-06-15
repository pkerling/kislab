class SpeedMonitor
{
  const int mPin;
  
  unsigned long mLastTime = 0;
  bool mLastState = 0;
  unsigned long mTurnTime = 0;
  unsigned short mFactor;
  bool mMeasureHalfCycles;

public:
  typedef void (*CallbackType)(unsigned long);
  
private:
  CallbackType mCallback = nullptr;
  
public:
  SpeedMonitor(int pin, unsigned short factor = 1, bool measureHalfCycles = false)
  : mPin(pin), mFactor(factor), mMeasureHalfCycles(measureHalfCycles)
  {}

  void setup()
  {
    pinMode(mPin, INPUT);
  }

  void loop()
  {
    int state = digitalRead(mPin);
    if (state != mLastState) {
      if (mMeasureHalfCycles || !state) {
        unsigned long now = micros();
        unsigned long dif = now - mLastTime;
        mTurnTime = dif * mFactor;
        debugprint("tt new: ");
        debugprintln(mTurnTime);
        mLastTime = now;
        if (mCallback) {
          mCallback(mTurnTime);
        }
      }
      mLastState = state;
    }
  }

  void setCallback(CallbackType callback)
  {
    mCallback = callback;
  }

  unsigned long turnTimeUS()
  {
    return mTurnTime;
  }

  unsigned long lastMeasurementTimeUS()
  {
    return mLastTime;
  }
};

