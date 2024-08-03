#pragma once
#include "Watchy_Watchytchi.h"
#include <stdlib.h>     //srand, rand
#include "SpeciesCode.h"
#include <cmath>
#include "ReadingMaterial.h"
#include "MoodIconConfig.h"
#include "IdleScenes.h"

#include "fonts/Tintin_Dialogue8pt7b.h"
#include "fonts/Tintin_Dialogue9pt7b.h"
#include "fonts/Tintin_Dialogue10pt7b.h"
#include "fonts/Tintin_Dialogue16pt7b.h"

const unsigned char *foodBerry_stages[7] = {img_FoodBerry_Stage0, img_FoodBerry_Stage1, img_FoodBerry_Stage2, img_FoodBerry_Stage3, 
  img_FoodBerry_Stage4, img_FoodBerry_Stage5, img_FoodBerry_Stage6};
  const unsigned char *foodCucumber_stages[7] = {img_FoodCucumberPlate_Stage0, img_FoodCucumberPlate_Stage1, img_FoodCucumberPlate_Stage2, 
  img_FoodCucumberPlate_Stage3, img_FoodCucumberPlate_Stage4, img_FoodCucumberPlate_Stage5, img_FoodCucumberPlate_Stage6};
const unsigned char* flower_stages[6] = { img_GrowingFlower1, img_GrowingFlower2, img_GrowingFlower3, 
  img_GrowingFlower4, img_GrowingFlower5, img_GrowingFlower6};
const unsigned char *menu_reset_press_stages[4] = {img_MenuIcon_ResetSave_Active, img_MenuIcon_ResetSave_ActivePress1, img_MenuIcon_ResetSave_ActivePress2, img_MenuIcon_ResetSave_ActivePress3};
const unsigned char *hotSpring_nums [10] = {img_HotSpringNum_0, img_HotSpringNum_1, img_HotSpringNum_2, img_HotSpringNum_3, img_HotSpringNum_4, img_HotSpringNum_5, img_HotSpringNum_6, img_HotSpringNum_7, img_HotSpringNum_8, img_HotSpringNum_9};
const unsigned char *hotSpring_nums_inner [10] = {img_HotSpringNum_0_Inner, img_HotSpringNum_1_Inner, img_HotSpringNum_2_Inner, img_HotSpringNum_3_Inner, img_HotSpringNum_4_Inner, img_HotSpringNum_5_Inner, img_HotSpringNum_6_Inner, img_HotSpringNum_7_Inner, img_HotSpringNum_8_Inner, img_HotSpringNum_9_Inner};

const float k_secDurationToFullyDepleteHunger = 4.f * 60.f * 60.f;
const int k_maxSecondsDeltaForUpdate = 5 * 60;
const float k_happinessFullyChangeDuration = 3 * 60 * 60;

DaisyHog hog = DaisyHog();
MugSnake snake = MugSnake();
DeerSlug deerSlug = DeerSlug();

const unsigned char *img_smallFontArr[10] = {
  img_smallFont_0,
  img_smallFont_1,
  img_smallFont_2,
  img_smallFont_3,
  img_smallFont_4,
  img_smallFont_5,
  img_smallFont_6,
  img_smallFont_7,
  img_smallFont_8,
  img_smallFont_9
};

const int k_numIdleScenes = 4;
IdleScene* idleScenes[k_numIdleScenes] =
{
  new IdleScene_Default(),
  new IdleScene_FloorActivity(new std::vector<const unsigned char*> {img_FloorActivityProp_Blocks1, img_FloorActivityProp_Blocks2, img_FloorActivityProp_Blocks3, img_FloorActivityProp_Blocks4}),
  new IdleScene_FloorActivity(new std::vector<const unsigned char*> {img_FloorActivityProp_Drawing1, img_FloorActivityProp_Drawing2, img_FloorActivityProp_Drawing3, img_FloorActivityProp_Drawing4}),
  new IdleScene_FloorActivity(new std::vector<const unsigned char*> {img_FloorActivityProp_AntHill}),
};

static float floatMap(float val, float fromLow, float fromHigh, float toLow, float toHigh, float precision = 1000.f)
{
  return (float)map(precision * val, precision * fromLow, precision * fromHigh, precision * toLow, precision * toHigh) / precision;
}

static float floatModulo(float val, float range)
{
  while (val >= range)
    val -= range;  
  return val;
}

void Watchytchi::drawWatchFace(){
  // For some reason we need to clear alarm 1, otherwise the watch updates every single frame
  if (Watchy::RTC.rtcType == DS3231) {
    Watchy::RTC.rtc_ds.clearAlarm(DS3232RTC::ALARM_1);
  }
  srand(makeTime(currentTime) + ++srandSeedModifier);

  /*# Load Data: #*/
  tryLoadSaveData(false);

  /*# Tick state! #*/
  tickCreatureState();

  // Execute the draw function appropriate to our current game state
  (this->*drawFuncsByState[(int)gameState])();

  tryWriteSaveData(false);
  executeScheduledVibration();
} 

void Watchytchi::handleButtonPress() {
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();

  if (IS_DOUBLE_TAP) {
    while (!sensor.getINT()) {
      // Wait until interrupt is cleared.
      // Otherwise it will fire again and again.
    }
  }
  RTC.read(currentTime);
  srand(makeTime(currentTime) + ++srandSeedModifier);
  tryLoadSaveData(false);
  // Call the active handle button press function depending on my gamestate
  auto didHandlePress = (this->*handleButtonFuncsByState[(int)gameState])(wakeupBit);

  if (didHandlePress)
    return;
  else
    Watchy::handleButtonPress();
}

bool hasLoadedThisWake = false;
void Watchytchi::tryLoadSaveData(bool force)
{
  if (hasLoadedThisWake)
  {
    DBGPrintF("Skipping redundant load within same frame!"); DBGPrintln();
    return;
  }

  // Only load if we haven't yet saved since the device last lost power or was flashed
  hasLoadedThisWake = true;
  if (!force && lastSaveTs > 0)
  {
    DBGPrintF("Skipping redundant load from NVS, still loading RTC!"); DBGPrintln();
    loadFromRTC(); // However, we always load from RTC
    return;
  }

  NVS.begin();
  lastUpdateTsEpoch = NVS.getInt(nvsKey_lastUpdateTsEpoch, -1);
  invertColors = 1 == NVS.getInt(nvsKey_invertColors, 0);
  species = (CreatureSpecies)NVS.getInt(nvsKey_species, 0);
  numSecondsAlive = NVS.getInt(nvsKey_numSecondsAlive, 0);
  hunger = NVS.getFloat(nvsKey_hunger, 1.f);
  foodHappy_rtc = NVS.getFloat(nvsKey_foodHappy, foodHappy.defaultValue);
  strokeHappy_rtc = NVS.getFloat(nvsKey_strokeHappy, strokeHappy.defaultValue);
  walkHappy_rtc = NVS.getFloat(nvsKey_walkHappy, walkHappy.defaultValue);
  poopHappy_rtc = NVS.getFloat(nvsKey_poopHappy, poopHappy.defaultValue);
  sleepHappy_rtc = NVS.getFloat(nvsKey_sleepHappy, sleepHappy.defaultValue);
  playmateHappy_rtc = NVS.getFloat(nvsKey_playmateHappy, playmateHappy.defaultValue);
  hotSpringsHappy_rtc = NVS.getFloat(nvsKey_hotSpringsHappy, hotSpringsHappy.defaultValue);
  readHappy_rtc = NVS.getFloat(nvsKey_readHappy, readHappy.defaultValue);
  badEndSeconds = NVS.getInt(nvsKey_badEndSeconds, 0);
  badEndShieldSeconds = NVS.getInt(nvsKey_badEndShieldSeconds, 0);
  hasPoop = 1 == NVS.getInt(nvsKey_hasPoop, 0);
  lastPoopHour = NVS.getInt(nvsKey_lastPoopHour, -1);
  nextAlertTs = NVS.getInt(nvsKey_nextAlertTs, -1);
  nextAlertType = (ScheduledAlertType)NVS.getInt(nvsKey_nextAlertType, 0);
  activePlaymate = (PlaymateSpecies)NVS.getInt(nvsKey_activePlaymate, (int)PlaymateSpecies::NoPlaymate);
  lastPlaymateJoinTs = NVS.getInt(nvsKey_lastPlaymateJoinTs, -1);
  befriendedPlaymatesMask = NVS.getInt(nvsKey_befriendedPlaymatesMask, 0);
  bestFriendPlaymate = (PlaymateSpecies)NVS.getInt(nvsKey_bestFriendPlaymate, (int)PlaymateSpecies::NoPlaymate);
  DBGPrintF("Loading!! lastUpdateTsEpoch "); DBGPrint(lastUpdateTsEpoch); DBGPrintln();
  DBGPrintF("Loaded nextAlertType "); DBGPrint(nextAlertType); DBGPrintln();
  
  // Now that our RTC backing values have loaded from RTC, apply them to anything that uses them
  loadFromRTC();
}

void Watchytchi::loadFromRTC()
{
  foodHappy.value = foodHappy_rtc;
  strokeHappy.value = strokeHappy_rtc;
  walkHappy.value = walkHappy_rtc;
  poopHappy.value = poopHappy_rtc;
  sleepHappy.value = sleepHappy_rtc;
  playmateHappy.value = playmateHappy_rtc;
  hotSpringsHappy.value = hotSpringsHappy_rtc;
  readHappy.value = readHappy_rtc;
  
  // Assign creature (TODO: convert to map or enum-linked array)
  if (species == CreatureSpecies::Hog)
    critter = &hog;
  else if (species == CreatureSpecies::Snake)
    critter = &snake;
  else if (species == CreatureSpecies::Deer)
    critter = &deerSlug;
  critter->owner = this;
}

void Watchytchi::tryWriteSaveData(bool force)
{
  // Save only every couple of hours to avoid wearing out flash memory
  const int k_saveInterval = 60 * 60 * 2; 
  if (!force && lastUpdateTsEpoch < lastSaveTs + k_saveInterval)
  {
    DBGPrintF("Skipping NVS save because I saved too recently, still saving to RTC!");
    DBGPrintln();
    writeToRTC(); // Still write to RTC
    return;
  }
  lastSaveTs = lastUpdateTsEpoch;

  /*# Save data #*/
  NVS.begin();
  NVS.setInt(nvsKey_lastUpdateTsEpoch, (int64_t)lastUpdateTsEpoch);
  DBGPrintF("Saving lastUpdateTsEpoch as "); DBGPrint(lastUpdateTsEpoch); DBGPrintln();
  NVS.setInt(nvsKey_invertColors, invertColors ? 1 : 0, false);
  NVS.setInt(nvsKey_species, (int)species, false);
  NVS.setInt(nvsKey_numSecondsAlive, numSecondsAlive, false);
  NVS.setFloat(nvsKey_hunger, hunger, false);
  NVS.setFloat(nvsKey_foodHappy, foodHappy.value, false);
  NVS.setFloat(nvsKey_strokeHappy, strokeHappy.value, false);
  NVS.setFloat(nvsKey_walkHappy, walkHappy.value, false);
  NVS.setFloat(nvsKey_poopHappy, poopHappy.value, false);
  NVS.setFloat(nvsKey_sleepHappy, sleepHappy.value, false);
  NVS.setFloat(nvsKey_playmateHappy, playmateHappy.value, false);
  NVS.setFloat(nvsKey_hotSpringsHappy, hotSpringsHappy.value, false);
  NVS.setFloat(nvsKey_readHappy, readHappy.value, false);
  NVS.setInt(nvsKey_badEndSeconds, badEndSeconds, false);
  NVS.setInt(nvsKey_badEndShieldSeconds, badEndShieldSeconds, false);
  NVS.setInt(nvsKey_hasPoop, hasPoop ? 1 : 0, false);
  NVS.setInt(nvsKey_lastPoopHour, lastPoopHour, false);
  NVS.setInt(nvsKey_nextAlertTs, nextAlertTs, false);
  NVS.setInt(nvsKey_nextAlertType, (int)nextAlertType, false);
  NVS.setInt(nvsKey_activePlaymate, (int)activePlaymate, false);
  NVS.setInt(nvsKey_lastPlaymateJoinTs, lastPlaymateJoinTs, false);
  NVS.setInt(nvsKey_befriendedPlaymatesMask, befriendedPlaymatesMask, false);
  NVS.setInt(nvsKey_bestFriendPlaymate, bestFriendPlaymate, false);
  auto didSave = NVS.commit();
  DBGPrintF("Save success? "); DBGPrint(didSave); DBGPrintln();
  
  writeToRTC();
}

void Watchytchi::writeToRTC()
{
  foodHappy_rtc = foodHappy.value;
  strokeHappy_rtc = strokeHappy.value;
  walkHappy_rtc = walkHappy.value;
  poopHappy_rtc = poopHappy.value;
  sleepHappy_rtc = sleepHappy.value;
  playmateHappy_rtc = playmateHappy.value;
  hotSpringsHappy_rtc = hotSpringsHappy.value;
  readHappy_rtc = readHappy.value;
}

void Watchytchi::resetSaveData()
{
  NVS.begin();
  lastUpdateTsEpoch = -1;
  NVS.setInt(nvsKey_lastUpdateTsEpoch, lastUpdateTsEpoch);
  menuIdx = 0;
  lastAdvanceIdxMinute = 0;
  invertColors = false;
  NVS.setInt(nvsKey_invertColors, invertColors);
  species = CreatureSpecies::Deer;
  NVS.setInt(nvsKey_species, (int)species, false);
  numSecondsAlive = 0;
  NVS.setInt(nvsKey_numSecondsAlive, numSecondsAlive, false);
  hunger = 1.f;
  NVS.setFloat(nvsKey_hunger, hunger, false);
  foodHappy_rtc = foodHappy.defaultValue;
  NVS.setFloat(nvsKey_foodHappy, foodHappy.defaultValue, false);
  strokeHappy_rtc = strokeHappy.defaultValue;
  NVS.setFloat(nvsKey_strokeHappy, strokeHappy.defaultValue, false);
  walkHappy_rtc = walkHappy.defaultValue;
  NVS.setFloat(nvsKey_walkHappy, walkHappy.defaultValue, false);
  poopHappy_rtc = poopHappy.defaultValue;
  NVS.setFloat(nvsKey_poopHappy, poopHappy.defaultValue, false);
  sleepHappy_rtc = sleepHappy.defaultValue;
  NVS.setFloat(nvsKey_sleepHappy, sleepHappy.defaultValue, false);
  playmateHappy_rtc = playmateHappy.defaultValue;
  NVS.setFloat(nvsKey_playmateHappy, playmateHappy.defaultValue, false);
  hotSpringsHappy_rtc = hotSpringsHappy.defaultValue;
  NVS.setFloat(nvsKey_hotSpringsHappy, hotSpringsHappy.defaultValue, false);
  readHappy_rtc = readHappy.defaultValue;
  NVS.setFloat(nvsKey_readHappy, readHappy.defaultValue, false);
  badEndSeconds = 0;
  NVS.setInt(nvsKey_badEndSeconds, badEndSeconds, false);
  badEndShieldSeconds = 0;
  NVS.setInt(nvsKey_badEndShieldSeconds, badEndShieldSeconds, false);
  hasPoop = false;
  NVS.setInt(nvsKey_hasPoop, 0, false);
  lastPoopHour = -1;
  NVS.setInt(nvsKey_lastPoopHour, lastPoopHour, false);
  gameState = GameState::BaseMenu;
  playAnim = false;
  idleAnimIdx = 0;
  isPeriodicAnim = false;
  lastHungerCryMinute = -1;
  lastAnimateMinute = 0;
  isStrokingLeftSide = false;
  nextAlertTs = -1;
  NVS.setInt(nvsKey_nextAlertTs, nextAlertTs, false);
  nextAlertType = ScheduledAlertType::None;
  NVS.setInt(nvsKey_nextAlertType, (int)nextAlertType, false);
  activePlaymate = PlaymateSpecies::NoPlaymate;
  NVS.setInt(nvsKey_activePlaymate, activePlaymate, false);
  lastPlaymateJoinTs = -1;
  NVS.setInt(nvsKey_lastPlaymateJoinTs, lastPlaymateJoinTs, false);
  befriendedPlaymatesMask = 0;
  NVS.setInt(nvsKey_befriendedPlaymatesMask, befriendedPlaymatesMask, false);
  bestFriendPlaymate = PlaymateSpecies::NoPlaymate;
  NVS.setInt(nvsKey_bestFriendPlaymate, bestFriendPlaymate, false);
  submenuIdx = 0;
  hasExecutedEnding = false;
  auto didSave = NVS.commit();

  loadFromRTC(); // Apply any RTC saved values (i.e. happy backing values)
}

void Watchytchi::tickCreatureState()
{
  /*# Track time changes #*/
  time_t currentEpochTime = makeTime(currentTime);
  if (lastUpdateTsEpoch <= 0)
    lastUpdateTsEpoch = currentEpochTime;
  auto timeDelta = (float)(currentEpochTime - lastUpdateTsEpoch);
  if (timeDelta < 0.f)
    timeDelta = 0.f;
#if !DISABLE_MAX_TIMESTAMP
  if (timeDelta > k_maxSecondsDeltaForUpdate)
    timeDelta = k_maxSecondsDeltaForUpdate;
#endif
  DBGPrintF("Current epoch time "); DBGPrint(currentEpochTime); DBGPrintF(", prev epoch time "); DBGPrint(lastUpdateTsEpoch); DBGPrintF(", delta of "); DBGPrint(timeDelta); DBGPrintln();
  lastUpdateTsEpoch = currentEpochTime;

  // Detect first time user and play the intro
  if (numSecondsAlive <= 0)
  {
    gameState = GameState::Intro;
    numSecondsAlive += timeDelta;
    return;  
  }

  // Calculate number of days alive
  numSecondsAlive += timeDelta;

  // Schedule alerts
  auto beforeAlertTs = nextAlertTs;
  /*# Interaction Alert #*/
  // Schedule a new alert if we don't have one scheduled, if our current alert is expired, or if it is too far in the future due to a time change
  if (nextAlertTs < 0 || currentEpochTime - nextAlertTs >= k_alertExpirationWindow  || nextAlertTs >= currentEpochTime + k_alertInterval * 2)
    scheduleNextAlert();
  // Reschedule alerts if we're unhappy
  if (getHappyTier() <= HappyTier::Sad)
    scheduleNextAlert();
  // If we have a forced alert that isn't already scheduled, schedule one
  if (FORCED_NEXT_EVENT != -1 && (!hasActiveAlert() || nextAlertType != (ScheduledAlertType)FORCED_NEXT_EVENT))
    scheduleNextAlert();

  // Automatically turn off lights if its daytime
  if (getTimeOfDay() != TimeOfDay::LateNight)
    invertColors = false;

  // Force good value for species enum
  if ((int)species < 0 || (int)species >= (int)CreatureSpecies::COUNT)
  {
    species = (CreatureSpecies)constrain((int)species, 0, (int)CreatureSpecies::COUNT - 1);
  }

  // Grow up into a bigger critter!
  if (species == CreatureSpecies::Deer && numSecondsAlive >= 2.5 * 24 * 60 * 60)
  {
    species = rand() % 2 == 0 ? CreatureSpecies::Hog : CreatureSpecies::Snake;
  }

  // Handle the ending!
  if (gameState != GameState::Ending)
  {
    // Force ending?
    if (FORCED_ENDING == 0 || FORCED_ENDING == 1)
    {
      gameState = GameState::Ending;
      // Force bad ending?
      if (FORCED_ENDING == 1)
        badEndSeconds = k_secondsThresholdForBadEnd * 2;
      // Endings are only defined for adults, so forcibly grow up if I'm not an adult
      if (species == CreatureSpecies::Deer)
      {
        species = rand() % 2 == 0 ? CreatureSpecies::Hog : CreatureSpecies::Snake;
        loadFromRTC();
      }
      
      return;
    }

    // Good end: reach a certain age without getting the bad end
    auto maximumAge = 7 * 24 * 60 * 60;
    if (numSecondsAlive >= maximumAge)
      gameState = GameState::Ending;
    else if (qualifiesForBadEnd())
      gameState = GameState::Ending;
  }
  if (gameState == GameState::Ending)
    return;

  // Stat atrophy is slower in the morning to limit cases where you wake up and your creature is miserable
  auto atrophySpeedModifier = currentTime.Hour > 10 ? 1.f : 0.5f;

  /*# Aprophy Hunger #*/
  auto oldHunger = hunger;
  auto hungerDelta = timeDelta / k_secDurationToFullyDepleteHunger * atrophySpeedModifier;
  if (getTimeOfDay() != TimeOfDay::LateNight)
    hunger -= hungerDelta;
  if (hunger < 0.f)
    hunger = 0.f;
  DBGPrintF("Hunger delta = "); DBGPrint(hungerDelta); DBGPrintF(", new hunger = "); DBGPrint(hunger); DBGPrintln();
  // Hunger alert:
  if (getTimeOfDay() == TimeOfDay::Daytime && hunger <= 0.1f && (lastHungerCryMinute == -1 || currentTime.Minute - lastHungerCryMinute >= 30 || currentTime.Minute < lastHungerCryMinute))
  {
    scheduleVibration(5, 50);
    lastHungerCryMinute = currentTime.Minute;
  }
  else if (oldHunger > 0.45f && hunger <= 0.45f)
  {
    scheduleVibration(3, 50);
  }
  if (hunger > 0.1f && lastHungerCryMinute != -1)
  {
    lastHungerCryMinute = -1;
  }

  /*# Manage Playmate state! #*/
  const int k_playmateStayDuration = 3 * 60 * 60;

  // Force debug playmate option
  if (FORCED_ACTIVE_PLAYMATE > (int)PlaymateSpecies::NoPlaymate)
  {
    activePlaymate = (PlaymateSpecies)FORCED_ACTIVE_PLAYMATE;
    lastPlaymateJoinTs = lastUpdateTsEpoch;
  }

  // Playmates leave after a bit
  if (hasActivePlaymate() && lastUpdateTsEpoch > lastPlaymateJoinTs + k_playmateStayDuration)
    activePlaymate = PlaymateSpecies::NoPlaymate;

  /*# Manage Idle Scene State #*/
  if (activeIdleSceneIdx == -1 || lastUpdateTsEpoch > lastChangeIdleSceneTs + currentIdleSceneDuration)
    chooseNewIdleScene();

  /*# Atrophy happiness! #*/
  auto oldHappyPercent = getHappyPercent();

  // Move happiness up or down depending on state:
  auto happyDeltaAmt = timeDelta / k_happinessFullyChangeDuration;
  
  // Not being asleep at night makes me unhappy
  if (isElectricLit())
    sleepHappy.AddTo(-happyDeltaAmt);

  // Except for sleep itself, happiness doesn't change while the creature sleeps
  if (getTimeOfDay() != TimeOfDay::LateNight || isElectricLit())
  {
    // If I'm cranky from a bad night of sleep, gradually have the mood wear off
    if (!isElectricLit())
      sleepHappy.MoveTowards(0.f, happyDeltaAmt);

    // Food happy goes up or down based on whether I'm fed
    if (hunger < 0.45f)
      foodHappy.AddTo(-happyDeltaAmt * atrophySpeedModifier);
    else if (hunger >= 0.6f)
      foodHappy.AddTo(happyDeltaAmt);

    // If starving, limit maximum food happpiness
    if (hunger <= 0.001f)
      foodHappy.value = constrain(foodHappy.value, foodHappy.min, foodHappy.max / 2.f);

    // Poop being out makes me unhappy
    if (hasPoop)
      poopHappy.AddTo(-happyDeltaAmt * atrophySpeedModifier);
    else
      poopHappy.MoveTowards(0, happyDeltaAmt);

    // Walk and stroke happy slowly fade to 0 over time if you haven't done those actions recently
    if (gameState != GameState::SharedWalk)
      walkHappy.MoveTowards(0, happyDeltaAmt * 0.25f);
    if (gameState != GameState::StrokingMode)
      strokeHappy.MoveTowards(0, happyDeltaAmt * 0.25f);

    // If I have a playmate, that makes me ambiently happy
    if (hasActivePlaymate())
      playmateHappy.AddTo(happyDeltaAmt);
    else
      playmateHappy.MoveTowards(0, happyDeltaAmt * 0.25f);

    // If I'm in the hot springs, I'm ambiently happy (TODO: instead focus on completing timers rather than ambient gain)
    if (gameState == GameState::HotSpringsTimer)
      hotSpringsHappy.AddTo(happyDeltaAmt);
    else
      hotSpringsHappy.MoveTowards(0, happyDeltaAmt * 0.25f);

    // If I'm not reading, atrophy reading happy
    if (gameState != GameState::Reading)
      readHappy.MoveTowards(0, happyDeltaAmt * 0.25f); 
  }

  lastHappyDelta = getHappyPercent() - oldHappyPercent;
  DBGPrintF("Happy Contributions: food "); DBGPrint(foodHappy.value); DBGPrintF(", stroke "); DBGPrint(strokeHappy.value); 
    DBGPrintF(", walk "); DBGPrint(walkHappy.value); DBGPrintF(", poopHappy "); DBGPrint(poopHappy.value); DBGPrintF(", sleep "); 
    DBGPrint(sleepHappy.value); DBGPrintF(", playmate "); DBGPrint(playmateHappy.value); DBGPrintF(", hot springs "); DBGPrint(hotSpringsHappy.value);DBGPrintF(", read "); DBGPrint(readHappy.value); DBGPrintln();
  DBGPrintF("New happyPercent "); DBGPrint(getHappyPercent()); DBGPrintF(", from old happy percent "); DBGPrint(oldHappyPercent); 
  DBGPrintF(", lastHappyDelta is "); DBGPrint(lastHappyDelta); DBGPrintln();

  /*# Tick running away #*/
  auto isContemplatingRunAway = getHappyPercent() <= 0.f && isUnhappinessIncreasing();
  // Gaining bad end points respects the atrophy speed modifier. Can't gain bad end points too late at night to prevent
  //  a run-away triggering simply by forgetting to turn off the lights one night
  auto badEndProgressDelta = timeDelta * atrophySpeedModifier;
  if (currentTime.Hour < 9)
    badEndProgressDelta = 0;

  // Offer a buffer "shield" that ticks up first before you get permanent run away points
  const int k_shieldSecondsSize = 30 * 60;
  if (isContemplatingRunAway)
    badEndShieldSeconds = constrain(badEndShieldSeconds + badEndProgressDelta, 0.f, k_shieldSecondsSize);  
  else
    badEndShieldSeconds = constrain(badEndShieldSeconds - timeDelta, 0.f, k_shieldSecondsSize);
  // Once the shield is used up, accumulate permanent bad points
  if (isContemplatingRunAway && badEndShieldSeconds >= k_shieldSecondsSize)
    badEndSeconds += badEndProgressDelta;
  // The bad end meter can go down just a tad if you're happy and hovering at the edge
  if (getHappyTier() == HappyTier::Blissful && badEndSeconds > k_secondsThresholdForBadEnd * 0.8f)
    badEndSeconds -= timeDelta;
  // Non-adults can't run away, so clamp if I'm a teen
  if (species == CreatureSpecies::Deer)
    badEndSeconds = constrain(badEndSeconds, 0, k_secondsThresholdForBadEnd * 0.8f);
  DBGPrintF("Contemplating RunAway? "); DBGPrint(isContemplatingRunAway); DBGPrintF(". Bad end seconds: "); DBGPrint(badEndSeconds); DBGPrintF(", shield seconds is "); DBGPrint(badEndShieldSeconds); DBGPrintln();

  /*# Atrophy poop: #*/
  if (!hasPoop && getTimeOfDay() != TimeOfDay::LateNight && (lastPoopHour == -1 || currentTime.Hour >= lastPoopHour + 4 || currentTime.Hour < lastPoopHour) 
    && lastAnimateMinute > 0)/*Hack: do this to avoid immediate poop at the start of a new game. */
  {
    DBGPrintF("Pooping! lastPoopHour = "); DBGPrint(lastPoopHour); DBGPrintF(", lastAnimateMinute = "); DBGPrint(lastAnimateMinute); DBGPrintln();
    hasPoop = true;
    lastPoopHour = currentTime.Hour;
    scheduleVibration(3, 50);

    // Random chance to add a playmate if I've already befriended one
    // HACK: we do this at the same time as pooping because it's roughly the same frequency that we want to check at
    if (!hasActivePlaymate() && befriendedPlaymatesMask != 0)
    {
      if (rand() % 4 == 0)
        chooseNewActivePlaymate(true);
    }
  }
  else
  {
    DBGPrintF("Chose not to poop! lastPoopHour = "); DBGPrint(lastPoopHour); DBGPrintF(", lastAnimateMinute = "); DBGPrint(lastAnimateMinute); DBGPrintln();
  }

  // Tick hot springs timer
  // TODO: move to state-specific tick function
  if (gameState == GameState::HotSpringsTimer && isHotSpringsTimerPlaying)
  {
    auto wasHotSpringsTimerExpired = hotSpringsTimerSecsLeft <= 0;
    hotSpringsTimerSecsLeft = max(0.f, hotSpringsTimerSecsLeft - timeDelta);
    if (hotSpringsTimerSecsLeft <= 0 && !wasHotSpringsTimerExpired)
    {
      if (isHotSpringsTimerOnBreak)
        scheduleVibration(3, 100);
      else
        scheduleVibration(5, 200);
    }
  }
}

TimeOfDay Watchytchi::getTimeOfDay()
{
  return getTimeOfDay(currentTime);
}

TimeOfDay Watchytchi::getTimeOfDay(const tmElements_t &tm)
{
  if (tm.Hour >= 21 || tm.Hour <= 7)
    return TimeOfDay::LateNight;
  else if (tm.Hour > 18 || (tm.Hour == 18 && tm.Minute >= 30))
    return TimeOfDay::Dusk;
  else
    return TimeOfDay::Daytime;
}


float Watchytchi::getHappyPercent(bool shouldConstrain)
{
  auto rawSum = foodHappy.value + strokeHappy.value + walkHappy.value 
    + poopHappy.value + sleepHappy.value + playmateHappy.value + hotSpringsHappy.value
    + readHappy.value;
  if (shouldConstrain)
    return constrain(rawSum, 0, 1);
  else
    return rawSum;
}

HappyTier Watchytchi::getHappyTier()
{
  return getHappyTier(getHappyPercent());
}

HappyTier Watchytchi::getHappyTier(float hPercent)
{
  if (hPercent >= 0.95f)
    return HappyTier::Blissful;
  if (hPercent >= 0.66f)
    return HappyTier::Happy;
  if (hPercent >= 0.33f)
    return HappyTier::Neutral;
  
  return HappyTier::Sad;
}


ContextAction Watchytchi::getActiveContextAction()
{
  // If it's late night, show the light switch
  if (!invertColors && getTimeOfDay() == TimeOfDay::LateNight)
    return ContextAction::LightsOff;
  // If it's daytime and I have poop, show clean icon
  else if (!invertColors && hasPoop)
    return ContextAction::Clean;
  // Otherwise, show the event selection
  else if (hasActiveAlert())
    return ContextAction::TriggerAlert;
  else
    return ContextAction::NoContextActionAvailable;
}

bool Watchytchi::isUnhappinessIncreasing()
{
  return hunger <= 0.45f || hasPoop; 
}

bool Watchytchi::qualifiesForBadEnd()
{
  return badEndSeconds >= k_secondsThresholdForBadEnd;
}

bool Watchytchi::isElectricLit()
{
  return getTimeOfDay() == TimeOfDay::LateNight && !invertColors;
}

void Watchytchi::chooseNewActivePlaymate(bool onlyBefriended)
{
  // If I have a best friend, they are more likely to join me
  DBGPrintF("=======Playmate Choice: seeded random with TS "); DBGPrint(lastUpdateTsEpoch);
  DBGPrintF(", best friend is "); DBGPrint(bestFriendPlaymate);
  if (bestFriendPlaymate != PlaymateSpecies::NoPlaymate && rand() % 2 == 0)
  {
    activePlaymate = bestFriendPlaymate;
    DBGPrintF(" and I chose to hang with them, ");
  }
  // Choose random playmate
  else
  {
    auto newPmIdx = rand() % (int)PlaymateSpecies::NUMPLAYMATES;
    DBGPrint("I chose a random playmate "); DBGPrint(newPmIdx); 
    // If I'm only supposed to choose befriended ones, budge the int until I choose a befriended playmate
    if (onlyBefriended && befriendedPlaymatesMask != 0)
    {
      while (0 == ((1 << newPmIdx) & befriendedPlaymatesMask))
      {
        newPmIdx = (newPmIdx + 1) % (int)PlaymateSpecies::NUMPLAYMATES;
        DBGPrintF(", shifted it to "); DBGPrint(newPmIdx); DBGPrintF(" to enforce only befriended, "); 
      }
    }
    activePlaymate = (PlaymateSpecies)newPmIdx;
    DBGPrintF(", and my final playmate is "); DBGPrint(newPmIdx);
  }

  lastPlaymateJoinTs = lastUpdateTsEpoch;
  befriendedPlaymatesMask |= (1 << (int)activePlaymate);

  if (bestFriendPlaymate == PlaymateSpecies::NoPlaymate)
  {
    // Potential of this playmate becoming my new best friend!
    if (rand() % 3 == 0)
    {
      DBGPrintF(", and I made them my new best friend!"); DBGPrintln();
      bestFriendPlaymate = activePlaymate; 
    }
  }
}

bool Watchytchi::hasActivePlaymate()
{
  return activePlaymate > PlaymateSpecies::NoPlaymate;
}

int Watchytchi::getPlaymateXOffset()
{
  if (idleScenes[activeIdleSceneIdx]->OffsetCreatureToLeft())
    return -65;
  if (hasActivePlaymate())
    return -50;
  return 0;
}

void Watchytchi::chooseNewIdleScene()
{
  activeIdleSceneIdx = rand() % k_numIdleScenes;
  idleScenes[activeIdleSceneIdx]->Begin();
}

void Watchytchi::clearCreatureBackground()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  display.fillRect(getPlaymateXOffset() + 100 - 36, 97, 156 - (100 - 36) + 8, 72, color_bg);

  if (hasActivePlaymate())
    display.fillRect(120, 94, 106, 72, color_bg);
}

void Watchytchi::clearNonUIBackground()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  display.fillRect(0, 33, 200, 134, color_bg);
}

void Watchytchi::clearScreen()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  display.fillRect(0, 0, 200, 200, color_bg);
}

void Watchytchi::drawUIButton(int idx, bool quickCursorUpdate)
{
    if (getTimeOfDay() != TimeOfDay::LateNight)
      invertColors = false;
    auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
    auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  
    int numButtons = 8;
    int width = 200;
    int height = 200;
    int buttonWidth = 32;
    int numPerRow = numButtons / 2;
    float separation = width / (float)numPerRow; 
    auto iconColor = isElectricLit() ? color_bg : color_fg;
    int column = idx % numPerRow;
    int xPos = (int)(width * ((float)column / numPerRow) + (separation / 2)) - buttonWidth / 2;
    int yPos;
    if (idx < numPerRow)
      yPos = 1;
    else
      yPos = height - buttonWidth - 2;

    // Cursor timeout
    if (currentTime.Minute - lastAdvanceIdxMinute >= 2 || (lastAdvanceIdxMinute > currentTime.Minute && currentTime.Minute >= 1))
        menuIdx = -1;

    if (quickCursorUpdate)
      display.fillRect(xPos, yPos, 32, 32, isElectricLit() ? color_fg : color_bg);

    auto selected = idx == menuIdx;
    if (idx == MENUIDX_INSPECT)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Status_Active : img_MenuIcon_Status_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_FEED)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Feed_Active : img_MenuIcon_Feed_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_STROKE)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Stroke_Active : img_MenuIcon_Stroke_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_CONTEXT)
    {
      // The context icon can be a number of things!
      auto contextAction = getActiveContextAction();

      if (contextAction == ContextAction::LightsOff)
        display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Lights_Active : img_MenuIcon_Lights_Inactive, 32, 32, iconColor);
      else if (contextAction == ContextAction::Clean)
        display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Clean_Active : img_MenuIcon_Clean_Inactive, 32, 32, iconColor);
      else if (contextAction == ContextAction::TriggerAlert)
          display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Alert_Active_HasNotification : img_MenuIcon_Alert_Inactive_HasNotification, 32, 32, iconColor);
      else
          display.drawBitmap(xPos, yPos, img_MenuIcon_Alert_Inactive, 32, 32, iconColor);
    }
    else if (idx == MENUIDX_WALK)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Walk_Active : img_MenuIcon_Walk_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_HOTSPRINGS)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_HotSprings_Active : img_MenuIcon_HotSprings_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_READ)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Read_Active : img_MenuIcon_Read_Inactive, 32, 32, iconColor);
    else if (idx == MENUIDX_SETTINGS)
      display.drawBitmap(xPos, yPos, selected ? img_MenuIcon_Settings_Active : img_MenuIcon_Settings_Inactive, 32, 32, iconColor);

    // Selected drop shadow
    if (idx == menuIdx)
    {
      if (idx < numPerRow) 
        display.drawBitmap(xPos, yPos + buttonWidth + 1, img_MenuIconShadow, 32, 2, iconColor);
      else
        display.drawBitmap(xPos, yPos + buttonWidth, img_MenuIconShadow, 32, 2, iconColor);

    }
}

void Watchytchi::drawAllUIButtons()
{
  for (auto i = 0; i < 8; i++)
  {
    drawUIButton(i, false);
  }
}

void Watchytchi::drawBgEnvironment()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  // Black or white background depending on time of day
  display.fillScreen(color_bg);

  // Draw some shaded radial gradients if it's evening or night but the lights are on
  if (isElectricLit())
    display.drawBitmap(0, 0, img_LightbulbLightGradient_1, 200, 200, color_fg);
  else if (getTimeOfDay() == TimeOfDay::Dusk)
    display.drawBitmap(0, 0, img_DuskLightGradient, 200, 200, color_fg);
  
  if (getTimeOfDay() == TimeOfDay::LateNight)
    display.drawBitmap(100 - (9/2), 0, img_HangingLightbulb, 9, 81, color_fg); 
}

void Watchytchi::drawWeather(){
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  
  // Position Sun / moon according to time of day:   
  bool fastDebug = false;
  float hourF;
  if (!fastDebug)
    hourF = (float)currentTime.Hour + (currentTime.Minute / 60.f) + (currentTime.Second / (60.f * 60.f));  
  else
    hourF = (float)((currentTime.Second % 48) / 2.f);

  float adjustedHour = floatModulo(hourF + 6.f, 12.f);
    
  float t = (adjustedHour  / 12.f);
  float theta = t * PI;

  float x = floatMap(cos(theta), -1.f, 1.f, 0, 1.f) * 200.f;
  float y = floatMap(1.f - abs(sin(theta)), 0.f, 1.f, 40.f, 125.f);

  display.drawBitmap(x - 21, y - 21, hourF <= 6 || hourF >= 18 ? img_Moon : img_Sun, 42, 42, color_fg);


  // Draw clouds
  auto cloud1T = floatModulo(t * 30.f, 1.f);
  float cloud1X = floatMap(cloud1T, 0.f, 1.f, 200.f, -100.f);
  auto cloud2T = floatModulo((t + 0.3) * 12.5f, 1.f);
  float cloud2X = floatMap(cloud2T, 0.f, 1.f, 400.f, -130.f);
  auto cloud3T = floatModulo((t + 0.7) * 2.5f, 1.f);
  float cloud3X = floatMap(cloud3T, 0.f, 1.f, 500.f, -300.f);

  auto isDark = currentTime.Hour > 18 || (currentTime.Hour == 18 && currentTime.Minute >= 30);
  display.drawBitmap(cloud1X, 42, isDark ? img_DarkCloud1 : img_Cloud1, 99, 54, color_fg);
  display.drawBitmap(cloud2X, 56, isDark ? img_DarkCloud2 : img_Cloud2, 128, 62, color_fg);
  display.drawBitmap(cloud3X, 30, isDark ? img_DarkCloud3 : img_Cloud3, 130, 35, color_fg);
}

bool Watchytchi::tryDrawMoodle(int &idx, const MoodIconConfig* icons, float happyLevel)
{
  if (abs(happyLevel) < 0.1)
    return false;

  const int k_topLeftX = 8;
  const int k_topLeftY = 21;
  const int k_moodleWidth = 32;
  const int k_moodlePadding = 2;
  const int k_numPerRow = 5;
  
  int k_columnIdx = idx % k_numPerRow;
  int xPos = k_topLeftX + k_moodleWidth * k_columnIdx + k_moodlePadding * max(0, k_columnIdx);
  int yPos = idx < k_numPerRow ? k_topLeftY : k_topLeftY + k_moodleWidth + 1;
  
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  // Draw border depending on mood
  if (happyLevel <= -0.2f)
    display.drawBitmap(xPos, yPos, img_MoodleBorder_ThunderCloud, 32, 32, color_fg);
  else if (happyLevel <= -0.1f)
    display.drawBitmap(xPos, yPos, img_MoodleBorder_SmallCloud, 32, 32, color_fg);
  else if (happyLevel >= 0.2f)
    display.drawBitmap(xPos, yPos, img_MoodleBorder_BigHeart, 32, 32, color_fg);
  else if (happyLevel >= 0.1f)
    display.drawBitmap(xPos, yPos, img_MoodleBorder_SmallHeart, 32, 32, color_fg);
    

  // TODO: don't hardcode for deerslug, create new function for life stage
  if (species == CreatureSpecies::Deer)
  {
    display.drawBitmap(xPos, yPos, happyLevel > 0 ? icons->img_BabyHappy : icons->img_BabySad, 32, 32, color_fg);
  }
  else
  {
    display.drawBitmap(xPos, yPos, happyLevel > 0 ? icons->img_AdultHappy : icons->img_AdultSad, 32, 32, color_fg);
  }
  // Draw mood icon

  idx++;
  return true;
}

void Watchytchi::drawIdleCreature(bool isAnimating){
  // Late night with lights on: Sleepy pose
  if (getTimeOfDay() == TimeOfDay::LateNight && !invertColors)
    critter->DrawSleepyPose(idleAnimIdx, isAnimating);
  // Late night lights off: Asleep pose
  else if (getTimeOfDay() == TimeOfDay::LateNight && invertColors)
    critter->DrawAsleepPose(idleAnimIdx, isAnimating);
  // Extreme hunger: Starving pose
  else if (hunger <= 0.1f)
    critter->DrawStarvingPose(idleAnimIdx, isAnimating);
  // Medium hunger: Hungry pose
  else if (hunger <= 0.45f)
    critter->DrawHungryPose(idleAnimIdx, isAnimating);
  // Unhappy: Sulking pose
  else if (getHappyTier() <= HappyTier::Sad)
    critter->DrawSadPose(idleAnimIdx, isAnimating);
  // Do a twich instead of the standing idle frames if we're on our periodic animation
  else if (isPeriodicAnim)
    critter->DrawTwitchAnimationPose(idleAnimIdx, isAnimating);
  // Default: Standing idle
  else
    critter->DrawIdlePose(idleAnimIdx, isAnimating);

  idleAnimIdx = (idleAnimIdx + 1) % 2;
}

void Watchytchi::drawEatAnim(){    
    auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
    auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
    int foodType;
    int numFrames;
    const int BERRIES = 0;
    const int CUCUMBER = 1;
    if (rand() % 2 == 0)
    {
      foodType = BERRIES;
      numFrames = 56;
    }
    else
    {
      foodType = CUCUMBER;
      numFrames = 28;
    }

     // Animate eating
     for (int i = 0; i < numFrames; i++)
     {
       critter->DrawEatingPose(i, true);
       display.drawBitmap(144 - 18 + getPlaymateXOffset(), 126, foodType == BERRIES ? foodBerry_stages[i / 8] : foodCucumber_stages[i / 4], 36, 36, color_fg);
       if (hasActivePlaymate())
        drawPlaymate(i);
       display.display(true);
       clearCreatureBackground();
     }

     //Hide Ghosting
     for(uint8_t i=0; i<3; i++){
       display.fillRect(100 - 36, 97, 120, 72, color_bg);
       drawIdleCreature(false);
       display.display(true);
     }
}

void Watchytchi::drawPlaymate(int idleIdx, int xOffset, int yOffset)
{
  if (!hasActivePlaymate())
    return;

  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  if (gameState == GameState::BaseMenu && idleScenes[activeIdleSceneIdx]->OffsetCreatureToLeft())
    xOffset += 10;

  DBGPrintF("Active playmate is "); DBGPrint(activePlaymate); DBGPrintln();

  // Draw the playmate!
  // TODO: make data classes for playmate instead of hardcoding
  if (activePlaymate == PlaymateSpecies::JuncoSnake)
  {
    if (gameState == GameState::Eating)
      display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_JuncoSnake_Eating1 : img_Playmate_JuncoSnake_Eating2, 106, 72, color_fg);
    else
      display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_JuncoSnake_Idle1 : img_Playmate_JuncoSnake_Idle2, 106, 72, color_fg);
  }
  else if (activePlaymate == PlaymateSpecies::SnappyLog)
    display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_SnappyLog_Idle1 : img_Playmate_SnappyLog_Idle2, 106, 72, color_fg);
  else if (activePlaymate == PlaymateSpecies::BugRat)
  {
    if (gameState == GameState::Eating)
      display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_BugRat_Eating1 : img_Playmate_BugRat_Eating2, 106, 72, color_fg);
    else
      display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_BugRat_Idle1 : img_Playmate_BugRat_Idle2, 106, 72, color_fg);
  }
  else if (activePlaymate == PlaymateSpecies::SmokerBird)
  {
    display.drawBitmap(xOffset + 120, yOffset + 94, idleIdx % 2 == 0 ? img_Playmate_CoolBird_Idle1 : img_Playmate_CoolBird_Idle2, 106, 72, color_fg);
  }
}

void Watchytchi::drawAgeFlower()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  // Draw a flower that grows a little bit every day:
  auto age = (int)(numSecondsAlive / (24 * 60 * 60));
  auto flowerGrowthIdx = constrain(age, 0, 5);
  display.drawBitmap(156, 91, flower_stages[flowerGrowthIdx], 30, 45, color_fg);
  DBGPrintF("numSecondsAlive is "); DBGPrint(numSecondsAlive); DBGPrintF(", age in days is "); DBGPrint(age); DBGPrintln();
}

void Watchytchi::drawDebugClock()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  // Draw a very small debug clock:
  //Hour
  display.drawBitmap(183-2, 195, img_smallFontArr[currentTime.Hour/10], 3, 5, color_fg); //first digit
  display.drawBitmap(187-2, 195, img_smallFontArr[currentTime.Hour%10], 3, 5, color_fg); //second digit

  //Minute
  display.drawBitmap(192-2, 195, img_smallFontArr[currentTime.Minute/10], 3, 5, color_fg); //first digit
  display.drawBitmap(196-2, 195, img_smallFontArr[currentTime.Minute%10], 3, 5, color_fg); //second digit
}

void Watchytchi::drawPoop()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  auto xPos = 32;
  if (idleScenes[activeIdleSceneIdx]->OffsetCreatureToLeft())
    xPos = -5;
  else if (hasActivePlaymate())
    xPos = 0;

  if (hasPoop)    
    display.drawBitmap(xPos, 200 - 32 - 20 - 4, idleAnimIdx % 2 == 0 ? img_SmallPoop_1 : img_SmallPoop_2, 20, 20, color_fg);
}

bool Watchytchi::dummy_handleButtonPress(uint64_t)
{
  return false;
}

void Watchytchi::baseMenu_draw()
{
  IdleScene* scene = idleScenes[activeIdleSceneIdx];
  scene->critter = critter;
  scene->owner = this;
  scene->DrawBG(idleAnimIdx);
  drawAllUIButtons();

  // Animate an idle loop every 3 minutes
  if (currentTime.Hour >= 6 && (currentTime.Minute - lastAnimateMinute > 3 || lastAnimateMinute > currentTime.Minute))
  {
    isPeriodicAnim = currentTime.Minute % 4 == 0;
    const int numAnimFrames = isPeriodicAnim ? 10 : 16;
    for (auto i = 0; i < numAnimFrames; i++)
    {
      isPeriodicAnim &= (i != numAnimFrames - 1);
      scene->DrawFG(idleAnimIdx);
      display.display(true);
      if (i != numAnimFrames - 1)
        clearNonUIBackground();
    }
    lastAnimateMinute = currentTime.Minute;
  }
  else {
    scene->DrawFG(idleAnimIdx);
  }    
}

bool Watchytchi::baseMenu_handleButtonPress(uint64_t wakeupBit)
{
  // Delay our 3 minute twitch animation so it doesn't happen while we're navigating in the menu
  lastAnimateMinute = currentTime.Minute;
  // Process selection
  if (IS_KEY_SELECT) {
    auto didPerformAction = false;
    // Open up the menu UI
    if (menuIdx == MENUIDX_INSPECT)
    {
      gameState = GameState::StatusCheck;
      didPerformAction = true;
    }
    // Start feeding
    if (menuIdx == MENUIDX_FEED)
    {
      gameState = GameState::Eating;
      didPerformAction = true;
    }
    // Start stroking
    if (menuIdx == MENUIDX_STROKE)
    {
      gameState = GameState::StrokingMode;
      DBGPrintF("Entering stroke mode!"); DBGPrintln();
    }      
    if (menuIdx == MENUIDX_CONTEXT)
    {
      auto contextAction = getActiveContextAction();
      if (contextAction == ContextAction::LightsOff)
      {
        invertColors = true;
        didPerformAction = true;
      }
      else if (contextAction == ContextAction::Clean)
      {
        hasPoop = false;
        poopHappy.value = poopHappy.max;
        auto prevHour = lastPoopHour;
        lastPoopHour = currentTime.Hour; // Cleaning resets last poop hour in order to prevent immediate poop once again
        lastAnimateMinute = -99; // Do a little dance afterwards by resetting the last animate minute
        DBGPrintF("Cleaned poop! New lastPoopHour ="); DBGPrint(lastPoopHour); DBGPrintF(", previously it was"); DBGPrint(prevHour); DBGPrintln();
        didPerformAction = true;
      }
      else if (contextAction == ContextAction::TriggerAlert)
      {
        gameState = GameState::AlertInteraction;
        didPerformAction = false;
      }
    }
    if (menuIdx == MENUIDX_WALK)
    {
      // TODO: move to state entry function
      bmaStepsAtWalkStart = sensor.getCounter();
      lastStepsDuringWalkCount = 0;
      gameState = GameState::SharedWalk;
      didPerformAction = true;
    }
    if (menuIdx == MENUIDX_HOTSPRINGS)
    {
      hotSpringsTimerSecsLeft = 25 * 60;
      isHotSpringsTimerPlaying = true;
      isHotSpringsTimerOnBreak = false;
      gameState = GameState::HotSpringsTimer;
    }
    if (menuIdx == MENUIDX_READ)
    {
      submenuIdx = 0;
      readingAssetIdx = rand() % k_numReadingAssets;
      gameState = GameState::Reading;
    }
    // Enter Settings submenu
    if (menuIdx == MENUIDX_SETTINGS)
    { 
      submenuIdx = 0;
      gameState = GameState::Settings;
      didPerformAction = true;
    }
    // Vibrate if this selection resulted in an action
    if (didPerformAction)
      vibrate(1, 50);
    showWatchFace(true);
    return true;
  }
 
  if (IS_KEY_CURSOR) {
    menuIdx = (menuIdx + 1) % 8;
    // Skip invalid care icons
    for (auto _ = 0; _ < 8; _++)
    {
      // At night, skip any care icons other than inspect, settings, or turning out the lights
      if (getTimeOfDay() == TimeOfDay::LateNight)
      {
        if (menuIdx == MENUIDX_INSPECT || menuIdx == MENUIDX_SETTINGS || ( menuIdx == MENUIDX_CONTEXT && getActiveContextAction() == ContextAction::LightsOff))
          break;
        else
          menuIdx = (menuIdx + 1) % 8;
      }
      // In the daytime, skip the context button if there is no contextual action
      else
      {
        if (menuIdx == MENUIDX_CONTEXT && getActiveContextAction() == ContextAction::NoContextActionAvailable)
          menuIdx = (menuIdx + 1) % 8;
        else
          break;
      }     
    }

    vibrate();
    lastAdvanceIdxMinute = currentTime.Minute;

    // Partial redraw -- commented out because it wasn't working
//    display.init(0, false); //_initial_refresh to false to prevent full update on init
//    display.setFullWindow();
//    showWatchFace(true);
//    //drawUIButton(menuIdx-1, true);
//    //drawUIButton(menuIdx, true);
//    //display.display(true); //partial refresh
//    // display.hibernate();
//    guiState = WATCHFACE_STATE;
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_CANCEL) {
    vibrate();
    menuIdx = MENUIDX_NOTHING;
    vibrate(1, 30);
    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::statusCheck_draw()
{
  drawBgEnvironment();
  drawIdleCreature(false);
  drawDebugClock();
  drawPoop();
  drawPlaymate(idleAnimIdx);
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  int moodleIdx = 0;
  tryDrawMoodle(moodleIdx, &MoodIconConfig::food, foodHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::stroke, strokeHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::walk, walkHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::poop, poopHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::sleep, sleepHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::playmate, playmateHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::hotSprings, hotSpringsHappy.value);
  tryDrawMoodle(moodleIdx, &MoodIconConfig::read, readHappy.value);

  auto happyTier = getHappyTier();
  if (happyTier == HappyTier::Blissful)
    display.drawBitmap(85, 80, img_HappinessMoodle_Blissful, 30, 30, color_fg);
  else if (happyTier == HappyTier::Happy)
    display.drawBitmap(85, 80, img_HappinessMoodle_Happy, 30, 30, color_fg);
  else if (happyTier == HappyTier::Neutral)
    display.drawBitmap(85, 80, img_HappinessMoodle_Neutral, 30, 30, color_fg);
  else
    display.drawBitmap(85, 80, img_HappinessMoodle_Sad, 30, 30, color_fg);

  // HappyContributor foodHappy = HappyContributor(0.25f,  -0.334f, 0.4f);
  // HappyContributor strokeHappy = HappyContributor(0.f,  0.f, 0.334f);
  // HappyContributor walkHappy = HappyContributor(0.f,  0.f, 0.334f);
  // HappyContributor poopHappy = HappyContributor(0.f,  -0.334f, 0.2f);
  // HappyContributor sleepHappy = HappyContributor(0.f,  -0.334f, 0.f);
  // HappyContributor playmateHappy = HappyContributor(0.f,  0.f, 0.2f);
  // HappyContributor hotSpringsHappy = HappyContributor(0.f,  0.f, 0.2f);
}

bool Watchytchi::statusCheck_handleButtonPress(uint64_t wakeupBit)
{
  if (IS_KEY_CURSOR || IS_KEY_SELECT || IS_KEY_CANCEL)
  {
    gameState = GameState::BaseMenu;
    return true;
  }

  return false;
}

void Watchytchi::eating_draw()
{
  drawBgEnvironment();
  drawWeather();
  drawDebugClock();
  drawPoop();
  drawEatAnim();
  
  // After drawing the eat animation, exit the eating state and draw the default screen
  gameState = GameState::BaseMenu;
  hunger = 1.f;
  baseMenu_draw();
}

void Watchytchi::stroking_draw()
{
  drawBgEnvironment();
  drawWeather();
  drawDebugClock();
  drawPoop();
  critter->DrawStrokingPose(idleAnimIdx, false);
  idleAnimIdx = (idleAnimIdx + 1) % 2;
  if (isStrokingLeftSide)
    display.drawBitmap(194, 0, img_StrokingButtonPrompt_R, 6, 46, GxEPD_BLACK);
  else
    display.drawBitmap(0, 0, img_StrokingButtonPrompt_L, 6, 46, GxEPD_BLACK);
  // If this stroke contributed to my happiness, draw a little heart
  if (strokeHappy.value < strokeHappy.max - 0.01f)
    display.drawBitmap(125, 115, isStrokingLeftSide ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, GxEPD_BLACK);
  
  if (hasActivePlaymate())
    drawPlaymate(idleAnimIdx);
  else
    drawAgeFlower();
}

bool Watchytchi::stroking_handleButtonPress(uint64_t wakeupBit)
{
  // In stroking mode, the left (cursor) and right (select) button moves the hand back and forth
  auto didPet = false;
  if (IS_KEY_SELECT && isStrokingLeftSide) 
  {
    isStrokingLeftSide = false;
    didPet = true;
  }

  if (IS_KEY_CURSOR && !isStrokingLeftSide) 
  {
      isStrokingLeftSide = true;
      didPet = true;
  }

  if (IS_KEY_SELECT || IS_KEY_CURSOR)
  {
    if (didPet)
    {
      strokeHappy.AddTo(0.0334f);
      vibrate(1, 30);
    }
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_CANCEL) {
    gameState = GameState::BaseMenu;
    vibrate(1, 50);
    showWatchFace(true);
    return true;
  }

  return false;
}

void Watchytchi::sharedWalk_draw()
{
  drawBgEnvironment();
  drawWeather();

  // Draw the critter in the center!
  critter->DrawWalkingPose(idleAnimIdx, false);
  if (hasActivePlaymate())
    drawPlaymate(idleAnimIdx);
  idleAnimIdx = (idleAnimIdx + 1) % 2;

  // Draw a row of flowers representing the player's walking progress
  const float k_walkStepDuration = 2000.f;
  auto stepsDuringWalk = sensor.getCounter() - bmaStepsAtWalkStart;
  auto stepPercent = (float)stepsDuringWalk / k_walkStepDuration;
  const int flowerWidth = 25;
  srand(currentTime.Day);
  for (auto i = 0; i < 200 / flowerWidth; i++)
  {
    auto idxT = (float)(i + 1.f) / (200.f / flowerWidth);

    // If the player has gotten this far, draw a flower (random with day as seed for continuity)
    auto diceRoll = rand() % 3;
    if (stepPercent >= idxT)
    {
      display.drawBitmap(i * flowerWidth, 170, diceRoll == 0 ? img_WalkingFlower_1 
        : (diceRoll == 1 ? img_WalkingFlower_2 : img_WalkingFlower_3), 25, 30, GxEPD_BLACK);
    }
    // Otherwise, draw an un-bloomed bud
    else
      display.drawBitmap(i * flowerWidth, 170, img_WalkingFlower_Bud, 25, 30, GxEPD_BLACK);
  }
  srand(makeTime(currentTime) + ++srandSeedModifier);

  // Draw raw # of steps (TODO: decide whether this is debug or permanent)
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(140, 30);
  display.println(stepsDuringWalk);

  // Add to happiness at pre-determined intervals
  const int k_walkHappyPayoutIncrements = 8;
  auto lastStepPercent = lastStepsDuringWalkCount / k_walkStepDuration;
  auto lastIncrementValue = (int)(lastStepPercent * k_walkHappyPayoutIncrements);
  auto newIncrementValue  = (int)(stepPercent * k_walkHappyPayoutIncrements);
  if (newIncrementValue > lastIncrementValue)
  {
    auto happinessToAdd = walkHappy.max * 0.75f / k_walkHappyPayoutIncrements;
    if (newIncrementValue == k_walkHappyPayoutIncrements)
      happinessToAdd += walkHappy.max * 0.25f; // Extra payout at the end
    walkHappy.AddTo(happinessToAdd);

    // Every time we add happiness, we have a chance to add a playmate
    auto shouldGetPlaymate = activePlaymate == PlaymateSpecies::NoPlaymate && rand() % 12 == 0;
    if (shouldGetPlaymate)
    {
      chooseNewActivePlaymate(false);
    }    
  }

  lastStepsDuringWalkCount = stepsDuringWalk;

  // Once you bloom all of the flowers, (+ margin so the player can see their work), exit the walk
  // TODO: instead, linger on some kind of succes screen until user input
  if (stepPercent >= 1.02f)
  {
    gameState = GameState::BaseMenu;
  }
}

bool Watchytchi::sharedWalk_handleButtonPress(uint64_t wakeupBit)
{
  // Redraw on button presses so the player can refresh their step count
  if (IS_KEY_CURSOR || IS_KEY_SELECT) 
  {
    showWatchFace(true);
    return true;
  }

  // Cancel exits the walk
  if (IS_KEY_CANCEL)
  {
    gameState = GameState::BaseMenu;
    showWatchFace(true);
    return true;
  }

  return false;
}

void Watchytchi::settings_draw()
{
  const int SIDX_SAVE = 0, SIDX_RESET = 1, NUMSETTINGS = 2;
  drawBgEnvironment();
  drawWeather();
  drawDebugClock();

  // Draw Settings options
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  display.drawBitmap(61, 83, submenuIdx == SIDX_SAVE ? img_MenuIcon_Save_Active : img_MenuIcon_Save_Inactive, 32, 32, color_fg);
  // Different Reset icon depending on num presses
  if (submenuIdx == SIDX_RESET)
    display.drawBitmap(98, 83, menu_reset_press_stages[constrain(numResetPresses, 0, 4)], 32, 32, color_fg);
  else
    display.drawBitmap(98, 83, img_MenuIcon_ResetSave_Inactive, 32, 32, color_fg);
  auto cursorX = 70 + 38 * submenuIdx;
  display.drawBitmap(cursorX, 69, img_MoodSelectionCursor, 12, 12, color_fg);
  
  drawIdleCreature(false);
  drawPlaymate(idleAnimIdx);
  drawPoop();
}

bool Watchytchi::settings_handleButtonPress(uint64_t wakeupBit)
{
  const int SIDX_SAVE = 0, SIDX_RESET = 1, NUMSETTINGS = 2;
  if (IS_KEY_CURSOR)
  {
    submenuIdx = (submenuIdx + 1) % NUMSETTINGS;
    numResetPresses = 0;
    scheduleVibration(1);
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_SELECT)
  {
    if (submenuIdx == SIDX_SAVE)
    {
      // Manual save for use before flashing
      tryWriteSaveData(true); 
    }

    else if (submenuIdx == SIDX_RESET)
    {
      numResetPresses++;
      if (numResetPresses >= 4)
        resetSaveData();
    }
    showWatchFace(true);
    return true;
  }

  // Cancel exits the submenu
  if (IS_KEY_CANCEL)
  {
    numResetPresses = 0;
    gameState = GameState::BaseMenu;
    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::hotSpringsTimer_draw()
{
  // drawBgEnvironment();
  clearScreen();
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  if (isHotSpringsTimerOnBreak)
  {
    display.drawBitmap(0, 0, img_HotSpringsBackground_Rest, 200, 200, color_fg);  
    critter->DrawHotSpringsPose(idleAnimIdx, true);
    if (hasActivePlaymate())
      drawPlaymate(idleAnimIdx, -100, -10);
  }
  else
  {
    idleAnimIdx++;
    display.drawBitmap(0, 0, img_HotSpringsBackground_Focused, 200, 200, color_fg);  
    critter->DrawHotSpringsPose(idleAnimIdx, false);
    if (hasActivePlaymate())
      drawPlaymate(idleAnimIdx, 25, -10);
  }
  

  auto roundedTimer = ceil(hotSpringsTimerSecsLeft / 60.f);
  int tensDigit = floor(roundedTimer / 10);
  int onesDigit = (int)roundedTimer % 10;

  if (isHotSpringsTimerPlaying)
  {
    if (isHotSpringsTimerOnBreak)
    {
      display.drawBitmap(19, 67, img_HotSpringWaterSymbol, 70, 48, color_fg);
      display.drawBitmap(19, 67, img_HotSpringWaterSymbol_Inner, 70, 48, color_bg);
    }
    else
    {
      display.drawBitmap(19, 67, img_HotSpringHeatSymbol, 70, 48, color_fg);
      display.drawBitmap(19, 67, img_HotSpringHeatSymbol_Inner, 70, 48, color_bg);
    }
  }
  else
  {
    display.drawBitmap(19, 67, img_HotSpringPauseSymbol, 70, 48, color_fg);
    display.drawBitmap(19, 67, img_HotSpringPauseSymbol_Inner, 70, 48, color_bg);
  }
  display.drawBitmap(80, 60, hotSpring_nums[tensDigit], 50, 60, color_fg);
  display.drawBitmap(80, 60, hotSpring_nums_inner[tensDigit], 50, 60, color_bg);
  display.drawBitmap(124, 60, hotSpring_nums[onesDigit], 50, 60, color_fg);
  display.drawBitmap(124, 60, hotSpring_nums_inner[onesDigit], 50, 60, color_bg);
  DBGPrintF("tens digit is "); DBGPrint(tensDigit); DBGPrintF(", ones digit is "); DBGPrint(onesDigit); DBGPrintln();
  DBGPrintF("Hot Springs seconds left: "); DBGPrint(hotSpringsTimerSecsLeft); DBGPrintln();

  if (isHotSpringsMenuOpen)
  {
    display.drawBitmap(2, 6, img_HotSpringsMenuIcon_WaterBottle, 13, 20, color_fg);
    display.drawBitmap(16, 6, img_HotSpringsMenuIcon_IceCube, 13, 20, color_fg);
    display.drawBitmap(30, 6, img_HotSpringsMenuIcon_Reset, 13, 20, color_fg);
    display.drawBitmap(44, 6, isHotSpringsTimerOnBreak ? img_HotSpringsMenuIcon_JumpToFocus : img_HotSpringsMenuIcon_JumpToBreak, 13, 20, color_fg);
    display.drawBitmap(5 + submenuIdx * 14, 0, img_HotSpringsMenuCursor, 6, 5, color_fg);
    display.drawBitmap(187, 7, img_HotSpringsCornerSelect, 11, 11, color_fg);
  }
  else if (hotSpringsTimerSecsLeft <= 0)
    display.drawBitmap(187, 7, isHotSpringsTimerOnBreak ? img_HotSpringsMenuIcon_JumpToFocus : img_HotSpringsMenuIcon_JumpToBreak, 13, 20, color_fg);
  else if (isHotSpringsTimerPlaying)
    display.drawBitmap(187, 7, img_HotSpringsCornerPause, 10, 13, color_fg);
  else if (!isHotSpringsTimerPlaying)
    display.drawBitmap(187, 7, img_HotSpringsCornerPlay, 11, 11, color_fg);
}

bool Watchytchi::hotSpringsTimer_handleButtonPress(uint64_t wakeupBit)
{
  // DEBUG: change timer left
  if (IS_KEY_CURSOR)
  {
    if (!isHotSpringsMenuOpen)
    {
      isHotSpringsMenuOpen = true;
      submenuIdx = 0;
    }
    else
    {
      submenuIdx = (submenuIdx + 1) % 4;
    }
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_SELECT)
  {
    if (isHotSpringsMenuOpen)
    {
      int SUBIDX_Add1Minute = 0, SUBIDX_ADDManyMinutes = 1, SUBIDX_ResetTimer = 2, SUBIDX_SwitchToBreak = 3;
      if (submenuIdx == SUBIDX_Add1Minute)
        hotSpringsTimerSecsLeft += 60;
      else if (submenuIdx == SUBIDX_ADDManyMinutes)
        hotSpringsTimerSecsLeft += 5 *60;
      else if (submenuIdx == SUBIDX_ResetTimer)
        hotSpringsTimerSecsLeft = isHotSpringsTimerOnBreak ? 5 * 60 : 25 * 60;
      else if (submenuIdx == SUBIDX_SwitchToBreak)
      {
        isHotSpringsTimerOnBreak = !isHotSpringsTimerOnBreak;
        hotSpringsTimerSecsLeft = isHotSpringsTimerOnBreak ? 5 * 60 : 25 * 60;
      }

    }
    // Pressing select while the timer is active will toggle pause
    else if (hotSpringsTimerSecsLeft > 0)
      isHotSpringsTimerPlaying = !isHotSpringsTimerPlaying;
    // Pressing select with an expired timer will begin or end the break
    else
    {
      isHotSpringsTimerOnBreak = !isHotSpringsTimerOnBreak;
      hotSpringsTimerSecsLeft = isHotSpringsTimerOnBreak ? 5 * 60 : 25 * 60;
    }
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_CANCEL)
  {
    // Cancel exits the submenu if open
    if (isHotSpringsMenuOpen)
      isHotSpringsMenuOpen = false;
    // Otherwise, exit hot springs
    else
    {
      gameState = GameState::BaseMenu;
    }

    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::reading_draw()
{
  auto readingAsset = allReadingAssets[readingAssetIdx];
  auto readingAssetLength = allReadingAssetLengths[readingAssetIdx];

  drawBgEnvironment();
  display.setFont(&Tintin_Dialogue10pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(14, 20);
  display.setTextWrap(true);
  display.println(readingAsset[submenuIdx]);

  display.drawBitmap(0, 160, img_ReadingFirstPersonBookEdge, 200, 40, GxEPD_BLACK);

  if (submenuIdx > 0)
    display.drawBitmap(-2, 0, img_StrokingButtonPrompt_L, 6, 46, GxEPD_BLACK);
  if (submenuIdx < readingAssetLength - 1)
    display.drawBitmap(196, 0, img_StrokingButtonPrompt_R, 6, 46, GxEPD_BLACK);

  critter->DrawReadingPose(idleAnimIdx, false);
  drawPlaymate(idleAnimIdx);
  drawPoop();
}

bool Watchytchi::reading_handleButtonPress(uint64_t wakeupBit)
{
  auto readingAssetLength = allReadingAssetLengths[readingAssetIdx];

  if (IS_KEY_CURSOR)
  {
    submenuIdx = max(submenuIdx - 1, 0);
    idleAnimIdx++;
    readHappy.AddTo(0.05f);
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_SELECT)
  {
    submenuIdx = min(submenuIdx + 1, readingAssetLength - 1);
    idleAnimIdx++;
    readHappy.AddTo(0.05f);
    showWatchFace(true);
    return true;
  }

  if (IS_KEY_CANCEL)
  {
    gameState = GameState::BaseMenu;
    lastAnimateMinute = -1;
    showWatchFace(true);
    return true;
  }
  return false;
}
