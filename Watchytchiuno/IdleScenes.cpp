#pragma once
#include "IdleScenes.h"
#include "Watchy_Watchytchi.h"
#include "SpeciesCode.h"

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

bool IdleScene_Default::IsEligible()
{
  return true;
}
