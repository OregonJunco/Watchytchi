#pragma once
#include "IdleScenes.h"
#include "Watchy_Watchytchi.h"
#include "SpeciesCode.h"

void IdleScene::Begin()
{
  // Random duration between lowest and highest
  auto t = (rand() % 1000) / 1000.f;
  currentIdleSceneDuration = (int)(GetDurationLowRange() + t * (GetDurationHighRange() - GetDurationLowRange()));
  lastChangeIdleSceneTs = lastUpdateTsEpoch; 
}

void IdleScene::Tick()
{

}

int IdleScene::GetTimeInScene()
{
  return lastUpdateTsEpoch - lastChangeIdleSceneTs;
}

float IdleScene::GetProgressT()
{
  return (float)GetTimeInScene() / (float)currentIdleSceneDuration; 
}

int IdleScene::GetFrameByProgress(int maxFrames)
{
  DBGPrintF("GetFrameByProgress: TimeInScene is "); DBGPrint(GetTimeInScene()); DBGPrintF(", ProgressT is");  DBGPrint(GetProgressT()); 
    DBGPrintF(", maxFrames is "); DBGPrint(maxFrames); DBGPrintF(", Output is "); DBGPrint(floor(GetProgressT() * maxFrames)); DBGPrintln();
  return min(maxFrames - 1, (int)floor(GetProgressT() * maxFrames));
}

float IdleScene::GetDurationLowRange()
{
#if ULTRA_SHORT_IDLESCENES
  return 20.f;
#endif

  return 1.5f * 60.f * 60.f; 
}

float IdleScene::GetDurationHighRange()
{
#if ULTRA_SHORT_IDLESCENES
  return 20.f;
#endif
  return 3.f * 60.f * 60.f; 
}

void IdleScene_Default::DrawBG(int idleIdx)
{
  owner->drawBgEnvironment();
  owner->drawWeather();
  owner->drawDebugClock();
  if (!owner->hasActivePlaymate())
    owner->drawAgeFlower();
}

void IdleScene_Default::DrawFG(int idleIdx, bool isAnimating)
{
  owner->drawPoop();
  owner->drawIdleCreature(isAnimating);
  if (owner->hasActivePlaymate())
    owner->drawPlaymate(idleIdx);
}

IdleScene_FloorActivity::IdleScene_FloorActivity(std::vector<const unsigned char*>* frames)
{
  this->framesA = frames;
  this->framesB = frames;
}

IdleScene_FloorActivity::IdleScene_FloorActivity(std::vector<const unsigned char *> *framesA, std::vector<const unsigned char *> *framesB)
{
  this->framesA = framesA;
  this->framesB = framesB;
}

void IdleScene_FloorActivity::DrawBG(int idleIdx)
{
  owner->drawBgEnvironment();
  owner->drawWeather();
  owner->drawDebugClock();
  if (!owner->hasActivePlaymate())
    owner->drawAgeFlower();
}

void IdleScene_FloorActivity::DrawFG(int idleIdx, bool isAnimating)
{
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  owner->drawPoop();
  owner->drawIdleCreature(isAnimating);
  if (owner->hasActivePlaymate())
    owner->drawPlaymate(idleIdx);
  auto frames = idleIdx % 2 == 0 ? framesA : framesB;

  auto xPos = owner->hasActivePlaymate() ? 68 : 68 + 30;
  owner->display.drawBitmap(xPos, 114, frames->at(GetFrameByProgress(frames->size())), 64, 56, color_fg);
  
}

bool IdleScene_FloorActivity::IsEligible()
{
  return owner->getTimeOfDay() != TimeOfDay::LateNight;
}

void IdleScene_Dream::DrawBG(int idleIdx)
{
  owner->drawBgEnvironment();
  owner->drawDebugClock();
  if (!owner->hasActivePlaymate())
    owner->drawAgeFlower();
}

void IdleScene_Dream::DrawFG(int idleIdx, bool isAnimating)
{
  auto color_fg = invertColors ? GxEPD_WHITE : GxEPD_BLACK;
  auto color_bg = invertColors ? GxEPD_BLACK : GxEPD_WHITE;

  owner->drawPoop();
  owner->drawIdleCreature(isAnimating);
  owner->display.drawBitmap(0, 0, idleIdx % 2 == 0 ? img_Idles_DreamBubble1 : img_Idles_DreamBubble2, 200, 200, color_fg);
  // Stabile random index within the hour
  srand(owner->currentTime.Hour + owner->currentTime.Day);
  auto dreamContentIdx = rand() % 4;
  if (dreamContentIdx == 0)
    owner->display.drawBitmap(62, 22, img_Playmate_JuncoSnake_Idle1, 106, 72, color_fg);
  else if (dreamContentIdx == 1)
    owner->display.drawBitmap(72, 15, img_Playmate_SnappyLog_Idle1, 106, 72, color_fg);
  else if (dreamContentIdx == 2)
    owner->display.drawBitmap(75, 25, img_Playmate_CoolBird_Idle1, 106, 72, color_fg);
  else if (dreamContentIdx == 3)
    owner->display.drawBitmap(80, 20, img_Playmate_BugRat_Idle1, 106, 72, color_fg);

  srand(makeTime(owner->currentTime) + ++srandSeedModifier);
}

bool IdleScene_Dream::IsEligible()
{
  return owner->getTimeOfDay() == TimeOfDay::LateNight && invertColors && !owner->hasActivePlaymate();
}
