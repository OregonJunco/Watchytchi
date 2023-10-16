#include "Watchy_Watchytchi.h"
#include <stdlib.h>     //srand, rand
#include "SpeciesCode.h"

bool Watchytchi::hasActiveAlert()
{
  time_t currentEpochTime = makeTime(currentTime);
  // The alert is active if it is scheduled in the past, but not so far in the past that we've missed it entirely
  return (nextAlertTs > 0 && currentEpochTime > nextAlertTs && currentEpochTime < nextAlertTs + k_alertExpirationWindow);
}

/// @brief Schedule the next "alert" to take place in the future, skipping nighttime alerts
void Watchytchi::scheduleNextAlert()
{
  tmElements_t alertTsElements;
  time_t currentEpochTime = makeTime(currentTime);
  nextAlertTs = currentEpochTime;

  if (FORCED_NEXT_EVENT > (int)ScheduledAlertType::None)
  {
    nextAlertType = (ScheduledAlertType)(FORCED_NEXT_EVENT);
    nextAlertTs += 1;
  }
  else
  {
    do
    {
      nextAlertTs += k_alertInterval;
      // Make sure we don't set an alert during the night:
      breakTime(nextAlertTs, alertTsElements);
    } while (getTimeOfDay(alertTsElements) == TimeOfDay::LateNight);

    if (getTimeOfDay(alertTsElements) == TimeOfDay::Dusk)
      nextAlertType = ScheduledAlertType::AskAboutDay;
    else
      nextAlertType = ScheduledAlertType::CloseUp;
  }
  DBGPrintF("Scheduled event! nextAlertType is"); DBGPrint(nextAlertType); DBGPrintF("NextAlertTs is "); DBGPrint(nextAlertTs); DBGPrintF("Epoch time is "); DBGPrint(currentEpochTime); DBGPrintln(); 
}

void Watchytchi::executeCloseUp()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  display.fillScreen(color_bg);
  // Draw top row of menu buttons (bottom is covered up by critter)
  for (auto i = 0; i < 4; i++)
    drawUIButton(i, false);

  // Draw the two close up frames back and forth
  const int numCloseUpFrames = 10;
  for (auto i = 0; i < numCloseUpFrames; i++)
  {
    critter->DrawCloseUpFrame(i, true);
    display.display(true);
    display.fillRect(0, 32, 200, 200 - 32, color_bg);
  }
}

void Watchytchi::poseHWYDQuestion()
{
  drawBgEnvironment();

  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;

  // First, pose the question:
  // TODO: Snake support
  critter->DrawPosingQuestionPose(idleAnimIdx, false);
  idleAnimIdx = (idleAnimIdx + 1) % 2;
  display.drawBitmap(69, 83, img_HowWasYourDayIcon, 32, 32, color_fg);
  display.drawBitmap(103, 85, img_MenuIcon_Placeholder_Active, 32, 32, color_fg);
  display.display(true);

  delay(2000);
}

void Watchytchi::executeHWYDResponse()
{
  // New Alert!
  scheduleNextAlert();

  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  const int EIDX_ANGRY = 0, EIDX_SAD = 1, EIDX_NEUTRAL = 2, EIDX_HAPPY = 3;

  switch(emotionSelectIdx)
  {
    case EIDX_ANGRY:
      // TODO: angry reaction with a lil knife
      lastAnimateMinute = -1;
      break;
    case EIDX_SAD:
      // TODO: sad reaction
      lastAnimateMinute = -1;
      break;
    case EIDX_NEUTRAL:
      // Do our little idle
      lastAnimateMinute = -1;
      break;
    case EIDX_HAPPY:
      // Do our happy close up!
      executeCloseUp();
      break;
  }
}

void Watchytchi::alertInteraction_draw()
{
  DBGPrintF("alertInteraction_draw! nextAlertType is "); DBGPrint(nextAlertType); DBGPrintln();
  auto beforeAlertTs = nextAlertTs;
  if (nextAlertType == ScheduledAlertType::CloseUp)
  {
    executeCloseUp();
    gameState = GameState::BaseMenu;
    scheduleNextAlert();
    // After executing the alert, draw the base menu
    baseMenu_draw();
  }
  else if (nextAlertType == ScheduledAlertType::AskAboutDay)
  {
    poseHWYDQuestion();
    gameState = GameState::HowWasYourDay;
    howWasYourDay_draw();
  }
  else
    gameState = GameState::BaseMenu;

  DBGPrintF("Next Alert ts was "); DBGPrint(beforeAlertTs); DBGPrintF(", now it is "); DBGPrint(nextAlertTs); DBGPrintln();
}

bool Watchytchi::howWasYourDay_handleButtonPress(uint64_t wakeupBit)
{
  if (IS_KEY_CURSOR) 
  {
      emotionSelectIdx = (emotionSelectIdx + 1) % 4;
      showWatchFace(true);
      return true;
  }
  else if (IS_KEY_SELECT)
  {
    executeHWYDResponse();
    gameState = GameState::BaseMenu;
    showWatchFace(true);
    lastAnimateMinute = -1;
    return true;
  }
  else if (IS_KEY_CANCEL)
  {
    gameState = GameState::BaseMenu;
    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::howWasYourDay_draw()
{
  drawBgEnvironment();
  drawWeather();
  drawDebugClock();

  // Draw happiness options
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  display.drawBitmap(37, 83, img_HappinessMoodle_Angry, 30, 30, color_fg);
  display.drawBitmap(69, 83, img_HappinessMoodle_Sad, 30, 30, color_fg);
  display.drawBitmap(101, 83, img_HappinessMoodle_Neutral, 30, 30, color_fg);
  display.drawBitmap(133, 83, img_HappinessMoodle_Happy, 30, 30, color_fg);
  auto cursorX = 46 + 32 * emotionSelectIdx;
  display.drawBitmap(cursorX, 69, img_MoodSelectionCursor, 12, 12, color_fg);
  
  drawIdleCreature(false);
  drawPoop();
}

bool Watchytchi::ending_handleButtonPress(uint64_t wakeupBit)
{
  if (IS_KEY_CURSOR || IS_KEY_SELECT || IS_KEY_CANCEL) 
  {
    hasExecutedEnding = true;
    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::ending_draw()
{
  if (!hasExecutedEnding)
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, img_GoodEnd_DaisyHog_PackedBags, 200, 200, GxEPD_BLACK);
  }
  else
  {
    /*# Ending animation! #*/

    // Phase 0: Show the packed bags a little longer
    vibrate(1, 30);
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, img_GoodEnd_DaisyHog_PackedBags, 200, 200, GxEPD_BLACK);
    display.display(true);
    delay(1000);

    // Phase 1: Hike up a mountain!
    vibrate(1, 50);
    for (auto i = 0; i < 10; i++)
    {
      display.fillScreen(GxEPD_WHITE);
      display.drawBitmap(0, 0, i % 2 == 0 ? img_GoodEnd_DaisyHog_Hike1 : img_GoodEnd_DaisyHog_Hike2, 200, 200, GxEPD_BLACK);
      display.display(true);
    }
    delay(2000);

    // Phase 2: Discover the wacky bonfire
    vibrate(1, 50);
    for (auto i = 0; i < 24; i++)
    {
      display.fillScreen(GxEPD_WHITE);
      display.drawBitmap(0, 0, i % 2 == 0 ? img_GoodEnd_DaisyHog_IntroBonfire1 : img_GoodEnd_DaisyHog_IntroBonfire2, 200, 200, GxEPD_BLACK);
      if (i < 3)
        display.fillScreen(GxEPD_BLACK);
      else if (i < 6)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut3, 200, 200, GxEPD_BLACK);
      else if (i < 9)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut2, 200, 200, GxEPD_BLACK);
      else if (i < 12)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut1, 200, 200, GxEPD_BLACK);
      display.display(true);
    }
    delay(2000);

    // Phase 3: Hug the caretaker goodbye :')
    vibrate(1, 50);
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, img_GoodEnd_DaisyHog_HugIntro, 200, 200, GxEPD_BLACK);
    display.display(true);
    delay(2000);
    for (auto i = 0; i < 10; i++)
    {
      display.fillScreen(GxEPD_WHITE);
      display.drawBitmap(0, 0, i % 2 == 0 ? img_GoodEnd_DaisyHog_Hugging1 : img_GoodEnd_DaisyHog_Hugging2, 200, 200, GxEPD_BLACK);
      display.display(true);
      vibrate(1, 10);
    }
    delay(2000);

    // Phase 4: Snuggling up with my new friends!
    vibrate(1, 50);
    for (auto i = 0; i < 24; i++)
    {
      display.fillScreen(GxEPD_WHITE);
      display.drawBitmap(0, 0, i % 2 == 0 ? img_GoodEnd_DaisyHog_OutroBonfire1 : img_GoodEnd_DaisyHog_OutroBonfire2, 200, 200, GxEPD_BLACK);
      
      if (i >= 21)
        display.fillScreen(GxEPD_BLACK);
      else if (i >= 18)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut3, 200, 200, GxEPD_BLACK);
      else if (i >= 15)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut2, 200, 200, GxEPD_BLACK);
      else if (i >= 12)
        display.drawBitmap(0, 0, img_GoodEnd_FadeOut1, 200, 200, GxEPD_BLACK);
      
      display.display(true);
    }
    delay(2000);
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(0, 0, img_GoodEnd_EndTitleCard, 200, 200, GxEPD_BLACK);
    display.display(true);
    delay(5000);
    hasExecutedEnding = false;
    resetSaveData();
    tryLoadSaveData(true);
  }
}
