//Derived from peerdavid's source at: https://github.com/peerdavid/Watchy
#include "Watchy_Base.h"

/*# State #*/
/*## State: System ##*/
RTC_DATA_ATTR int lastUpdateTsEpoch = -1;
RTC_DATA_ATTR int numResetPresses = 0;
RTC_DATA_ATTR int lastSaveTs = -1;

/*## State: UI ##*/
RTC_DATA_ATTR int menuIdx;
RTC_DATA_ATTR int lastAdvanceIdxMinute = 0;
RTC_DATA_ATTR bool invertColors = false;

/*## State: Saveable pet stats ##*/
RTC_DATA_ATTR CreatureSpecies species = CreatureSpecies::Deer;
RTC_DATA_ATTR int numSecondsAlive = 0;
RTC_DATA_ATTR float hunger = 1.f;
RTC_DATA_ATTR float foodHappy_rtc = 0.25f;
RTC_DATA_ATTR float strokeHappy_rtc = 0.f;
RTC_DATA_ATTR float walkHappy_rtc = 0.f;
RTC_DATA_ATTR float poopHappy_rtc = 0.f;
RTC_DATA_ATTR float sleepHappy_rtc = 0.f;
RTC_DATA_ATTR float playmateHappy_rtc = 0.f;
RTC_DATA_ATTR float hotSpringsHappy_rtc = 0.f;
RTC_DATA_ATTR float readHappy_rtc = 0.f;
RTC_DATA_ATTR bool hasPoop = false;
RTC_DATA_ATTR int lastPoopHour = -1;
RTC_DATA_ATTR int badEndSeconds = 0;
RTC_DATA_ATTR int badEndShieldSeconds = 0;

/*## State: Game State (General) ##*/
RTC_DATA_ATTR GameState gameState;
RTC_DATA_ATTR bool playAnim = false;
RTC_DATA_ATTR int idleAnimIdx = 0;
RTC_DATA_ATTR bool isPeriodicAnim = false;
RTC_DATA_ATTR int lastHungerCryMinute = -1;
RTC_DATA_ATTR int lastAnimateMinute = 0;
RTC_DATA_ATTR int activeIdleSceneIdx = 0;

/*## State: Game State (Stroking) ##*/
RTC_DATA_ATTR bool isStrokingLeftSide = false;

/*## State: Game State (Events & Alerts) ##*/
RTC_DATA_ATTR int nextAlertTs = -1;
RTC_DATA_ATTR ScheduledAlertType nextAlertType = ScheduledAlertType::None;
RTC_DATA_ATTR int submenuIdx = 0;
RTC_DATA_ATTR bool hasExecutedIntro = false;
RTC_DATA_ATTR bool hasExecutedEnding = false;

/*## State: Shared Walk ##*/
RTC_DATA_ATTR int bmaStepsAtWalkStart;
RTC_DATA_ATTR int lastStepsDuringWalkCount;

/*## State: Game State (Hot Springs Timer) ##*/
RTC_DATA_ATTR int hotSpringsTimerSecsLeft;
RTC_DATA_ATTR bool isHotSpringsTimerPlaying = true;
RTC_DATA_ATTR bool isHotSpringsTimerOnBreak = false;
RTC_DATA_ATTR bool isHotSpringsMenuOpen = false;


/*## State: Game State (Playmate) ##*/
RTC_DATA_ATTR PlaymateSpecies activePlaymate = PlaymateSpecies::NoPlaymate;
RTC_DATA_ATTR int lastPlaymateJoinTs = -1;
RTC_DATA_ATTR int befriendedPlaymatesMask = 0;
RTC_DATA_ATTR PlaymateSpecies bestFriendPlaymate = PlaymateSpecies::NoPlaymate;

/*## State: Game State (Reading) ##*/
RTC_DATA_ATTR int readingAssetIdx = 0;

uint8_t scheduledVibrationTimes;
uint32_t scheduledVibrationDelay;

void WatchyBase::handleButtonPress() {
  // Handled in base class
}
void WatchyBase::_rtcConfig() {
  RTC.read(currentTime);
}

void WatchyBase::scheduleVibration(uint8_t times, uint32_t delay_time) {
  if (times > scheduledVibrationTimes)
    scheduledVibrationTimes = times;
  if (delay_time > scheduledVibrationDelay)
    scheduledVibrationDelay = delay_time;
}

void WatchyBase::vibrate(uint8_t times, uint32_t delay_time) {
  // Ensure that no false positive double tap is produced
  sensor.enableFeature(BMA423_WAKEUP, false);

  pinMode(VIB_MOTOR_PIN, OUTPUT);
  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(VIB_MOTOR_PIN, true);
    delay(delay_time);
    digitalWrite(VIB_MOTOR_PIN, false);
    if (i != times - 1)
      delay(delay_time);
  }

  sensor.enableFeature(BMA423_WAKEUP, true);
}

void WatchyBase::executeScheduledVibration()
{
  if (scheduledVibrationTimes <= 0 || scheduledVibrationDelay <= 0)
    return;
  vibrate(scheduledVibrationTimes, scheduledVibrationDelay);
  scheduledVibrationTimes = 0;
  scheduledVibrationDelay = 0;
}

void WatchyBase::startProfile()
{
  DBGPrintF("startprofile!"); DBGPrintln();

  DBGPrintF("before starting a new profile, nextProfileIdx is "); DBGPrint(nextProfileIdx); DBGPrintln();
  profileMs[nextProfileIdx] = millis();
  nextProfileIdx++;
  DBGPrintF("after starting a new profile, nextProfileIdx is "); DBGPrint(nextProfileIdx); DBGPrintln();
}

void WatchyBase::endProfile(const char* label)
{
  DBGPrintF("Before endProfile, nextProfileIdx is "); DBGPrint(nextProfileIdx); DBGPrintln();

  auto idx = --nextProfileIdx;
  auto ms = millis();
  DBGPrintF("Profile label "); DBGPrint(label); DBGPrintF(" took "); DBGPrint(millis() - profileMs[idx]); DBGPrintF(" ms, "); 
  DBGPrintF("After endProfile, nextProfileIdx is "); DBGPrint(nextProfileIdx); DBGPrintln();
}

void WatchyBase::endProfileAndStart(const char* label)
{
  endProfile(label);
  startProfile();
}

HappyContributor::HappyContributor(float startValue, float min, float max)
{
  // this->value is set in load from RTC
  this->defaultValue = startValue;
  this->min = min;
  this->max = max;
}

void HappyContributor::AddTo(float delta)
{
  value = constrain(value + delta, min, max);
}

void HappyContributor::MoveTowards(float target, float absoluteDelta)
{
  if (value > target)
    value = constrain(value - absoluteDelta, target, max);
  else if (value < target)
    value = constrain(value + absoluteDelta, min, target);
}