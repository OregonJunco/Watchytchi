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
  return 30.f;
#endif

  return 1.5f * 60.f * 60.f; 
}

float IdleScene::GetDurationHighRange()
{
#if ULTRA_SHORT_IDLESCENES
  return 30.f;
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

void IdleScene_Default::DrawFG(int idleIdx)
{
  owner->drawPoop();
  owner->drawIdleCreature(idleIdx);
  if (owner->hasActivePlaymate())
    owner->drawPlaymate(idleIdx);
}

IdleScene_FloorActivity::IdleScene_FloorActivity(std::vector<const unsigned char*>* frames)
{
  this->frames = frames;
}

void IdleScene_FloorActivity::DrawBG(int idleIdx)
{
  owner->drawBgEnvironment();
  owner->drawWeather();
  owner->drawDebugClock();
  if (!owner->hasActivePlaymate())
    owner->drawAgeFlower();
}

void IdleScene_FloorActivity::DrawFG(int idleIdx)
{
  owner->drawPoop();
  owner->drawIdleCreature(idleIdx);
  if (owner->hasActivePlaymate())
    owner->drawPlaymate(idleIdx);
  owner->display.drawBitmap(68, 114, frames->at(GetFrameByProgress(frames->size())), 64, 56, GxEPD_BLACK);
  
}