#include "Watchy_Watchytchi.h"
#include <stdlib.h>     //srand, rand
#include "SpeciesCode.h"

/// @brief Schedule the next "alert" to take place in the future, skipping nighttime alerts
void Watchytchi::scheduleNextAlert()
{
  tmElements_t alertTsElements;
  time_t currentEpochTime = makeTime(currentTime);
  nextAlertTs = currentEpochTime;

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
  display.drawBitmap(100 - 36, 110, img_DaisyHog_PosingQuestion, 72, 55, color_fg);
  display.drawBitmap(69, 83, img_HowWasYourDayIcon, 32, 32, color_fg);
  display.drawBitmap(103, 85, img_MenuIcon_Placeholder_Active, 32, 32, color_fg);
  display.display(true);

  delay(2000);
}

void Watchytchi::drawHWYDMoodSelection()
{
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;

  display.drawBitmap(37, 83, img_HappinessMoodle_Angry, 30, 30, color_fg);
  display.drawBitmap(69, 83, img_HappinessMoodle_Sad, 30, 30, color_fg);
  display.drawBitmap(101, 83, img_HappinessMoodle_Neutral, 30, 30, color_fg);
  display.drawBitmap(133, 83, img_HappinessMoodle_Happy, 30, 30, color_fg);
  auto cursorX = 46 + 32 * emotionSelectIdx;
  display.drawBitmap(cursorX, 69, img_MoodSelectionCursor, 12, 12, color_fg);
}

void Watchytchi::executeHWYDResponse()
{
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