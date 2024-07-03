#include "Watchy_Watchytchi.h"
#include <stdlib.h>     //srand, rand
#include "SpeciesCode.h"

// Array of all bitmaps for convenience. (Total bytes used to store images in PROGMEM = 40192)
const int img_angryLetterFrames_size = 8;
const unsigned char* img_angryLetterFrames[8] = {
	img_BadEnd_AngryLetter1,
	img_BadEnd_AngryLetter2,
	img_BadEnd_AngryLetter3,
	img_BadEnd_AngryLetter4,
	img_BadEnd_AngryLetter5,
	img_BadEnd_AngryLetter6,
	img_BadEnd_AngryLetter7,
	img_BadEnd_AngryLetter8
};


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

  switch(submenuIdx)
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
      submenuIdx = (submenuIdx + 1) % 4;
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
  auto cursorX = 46 + 32 * submenuIdx;
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
    // Bad end: draw creature in running away pose
    if (qualifiesForBadEnd())
    {
      invertColors = false;
      clearScreen();
      drawWeather();
      drawAllUIButtons();
      drawDebugClock();
      critter->DrawRunningAwayIdle();
      drawAgeFlower();
    }
    // Good end: creature hangs out with packed bags. This is all prerendered
    else
    {
      display.fillScreen(GxEPD_WHITE);
      critter->DrawGoodEndPackedBags(0);
    }
  }
  /*# Ending animation! #*/
  else
  {
    // Bad end: run away
    if (qualifiesForBadEnd())
    {
      vibrate(1, 30);
      // Phase 0: walk offscreen
      for (auto i = 0; i < 10; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        critter->DrawRunningAwayWalking(i, i * 18);
        display.display(true);
        delay(300);
      }
      delay(1500);

      // Phase 1: Send an angry letter!
      for (auto i = 0; i < img_angryLetterFrames_size; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0, 0, img_angryLetterFrames[i], 200, 200, GxEPD_BLACK);
        display.display(true);
        if (i == 2)
          delay(1500);
        else if (i == 3)
          delay(1500);
        else
          delay(300);
      }
      delay(2000);

      // Phase 2: fade out on the letter:
      for (auto i = 0; i < 3; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(0, 0, img_angryLetterFrames[img_angryLetterFrames_size - 1], 200, 200, GxEPD_BLACK);

        if (i == 0)
          display.drawBitmap(0, 0, img_GoodEnd_FadeOut1, 200, 200, GxEPD_BLACK);
        if (i == 1)
          display.drawBitmap(0, 0, img_GoodEnd_FadeOut2, 200, 200, GxEPD_BLACK);
        if (i == 2)
          display.drawBitmap(0, 0, img_GoodEnd_FadeOut3, 200, 200, GxEPD_BLACK);
        display.display(true);
        delay(1000);
      }
      for (auto i = 0; i < 3; i++)
      {
        display.fillScreen(GxEPD_BLACK);
        display.display(true);
        delay(666);
      }

      // Phase 3: Ending title card
      display.fillScreen(GxEPD_WHITE);
      display.drawBitmap(0, 0, img_BadEnd_TitleCard, 200, 200, GxEPD_BLACK);
      display.display(true);
      delay(5000);
    }
    // Good end: hike up to the mountains and live with nice little freaks
    else
    {
      // Phase 0: Show the packed bags a little longer
      vibrate(1, 30);
      display.fillScreen(GxEPD_WHITE);
      critter->DrawGoodEndPackedBags(0);
      display.display(true);
      delay(1000);

      // Phase 1: Hike up a mountain!
      vibrate(1, 50);
      for (auto i = 0; i < 10; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        critter->DrawGoodEndHike(i % 2);
        display.display(true);
      }
      delay(2000);

      // Phase 2: Discover the wacky bonfire
      vibrate(1, 50);
      for (auto i = 0; i < 24; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        critter->DrawGoodEndIntroBonfire(i % 2);
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
      critter->DrawGoodEndHug(0);
      display.display(true);
      delay(2000);
      for (auto i = 0; i < 10; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        critter->DrawGoodEndHug(i % 2 == 0 ? 1 : 2);
        display.display(true);
        vibrate(1, 10);
      }
      delay(2000);

      // Phase 4: Snuggling up with my new friends!
      vibrate(1, 50);
      for (auto i = 0; i < 24; i++)
      {
        display.fillScreen(GxEPD_WHITE);
        critter->DrawGoodEndOutroBonfire(i % 2);
        
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
    }

    // After the ending, reset!
    hasExecutedEnding = false;
    resetSaveData();
    tryLoadSaveData(true);
  }
}

bool Watchytchi::intro_handleButtonPress(uint64_t wakeupBit)
{
  if (IS_KEY_CURSOR || IS_KEY_SELECT || IS_KEY_CANCEL) 
  {
    gameState = GameState::BaseMenu;
    showWatchFace(true);
    return true;
  }
  return false;
}

void Watchytchi::intro_draw()
{
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(0, 0, img_TitleCard, 200, 200, GxEPD_BLACK);
  delay(2000);
}
