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

float IdleScene::GetDurationLowRange()
{
  return 1.5f * 60.f * 60.f; 
}

float IdleScene::GetDurationHighRange()
{
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
