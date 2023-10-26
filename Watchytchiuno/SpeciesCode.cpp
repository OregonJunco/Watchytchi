#pragma once
#include "SpeciesCode.h"
#include "Watchy_Base.h"
#include "Watchy_Watchytchi.h"


/*# ==== DAISYHOG ====
        M
   ___<=8=>_
  /     V   \
 |        U  ===0
  \_________/    
    / \  / \
#*/

void DaisyHog::DrawStrokingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, isStrokingLeftSide ? img_DaisyHog_BeingStroked1 : img_DaisyHog_BeingStroked2, 72, 55, getColor_fg());
}
void DaisyHog::DrawEatingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DaisyHog_Eat1 : img_DaisyHog_Eat2, 72, 55, getColor_fg());
}
void DaisyHog::DrawSleepyPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DaisyHog_Sleepy1 : img_DaisyHog_Sleepy2, 72, 55, getColor_fg());
}
void DaisyHog::DrawAsleepPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DaisyHog_Sleep1 : img_DaisyHog_Sleep2, 72, 55, getColor_fg());
}
void DaisyHog::DrawStarvingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DaisyHog_VeryHungry1, 72, 55, getColor_fg());
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 110-16+7, idleIdx % 2 == 0 ? img_Emote_Hungry1 : img_Emote_Hungry2, 28, 19, getColor_fg());
  if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 85, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DaisyHog::DrawHungryPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DaisyHog_Hungry1, 72, 55, getColor_fg());
    if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 85, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DaisyHog::DrawSadPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DaisyHog_Sulking1 : img_DaisyHog_Sulking2, 72, 55, getColor_fg());
  if (owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 85, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DaisyHog::DrawTwitchAnimationPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DaisyHog_Twitch1 : img_DaisyHog_Twitch2, 80, 55, getColor_fg());
  if (owner->getHappyTier() >= HappyTier::Blissful)
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 119, 115, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
}
void DaisyHog::DrawIdlePose(int idleIdx, bool isAnimating)
{
  // Afternoon special: hind legs
  if (owner->getHappyTier() >= HappyTier::Happy && owner->currentTime.Hour >= 12 && owner->currentTime.Hour < 14)
  {
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 93 + 4, idleIdx % 2 == 0 ? img_DaisyHog_HindLegs1 : img_DaisyHog_HindLegs2, 72, 72, getColor_fg());
    if (isAnimating && owner->getHappyTier() >= HappyTier::Blissful)
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 95, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
  }
  // Every couple of hours: special idle
  else if (owner->getHappyTier() >= HappyTier::Happy && owner->currentTime.Hour % 2 == 0 && owner->currentTime.Minute >= 20 && owner->currentTime.Minute <= 40)
  {
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110 + 4, idleIdx % 2 == 0 ? img_DaisyHog_SkyGaze1 : img_DaisyHog_SkyGaze2, 72, 55, getColor_fg());
    if (!isAnimating || owner->getHappyTier() < HappyTier::Blissful)
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 112, 110, idleIdx % 2 == 0 ? img_Emote_Music1 : img_Emote_Music2, 28, 19, getColor_fg());
    else
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 112, 110, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
  }
  // Default: Standing idle
  else
  {
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleAnimIdx % 2 == 0 ? img_DaisyHog_Idle1 : img_DaisyHog_Idle2, 72, 55, getColor_fg());
    if (isAnimating && owner->getHappyTier() >= HappyTier::Blissful)
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 119, 115, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
  }
}

void DaisyHog::DrawWalkingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleAnimIdx % 2 == 0 ? img_DaisyHog_Walking1 : img_DaisyHog_Walking2, 72, 55, getColor_fg());
}

void DaisyHog::DrawPosingQuestionPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(100 - 36, 110, img_DaisyHog_PosingQuestion, 72, 55, getColor_fg());
}

void DaisyHog::DrawCloseUpFrame(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(0, 0, idleIdx % 2 == 0 ? img_CloseUp_Happy1 : img_CloseUp_Happy2, 200, 200, getColor_fg());
}

void DaisyHog::DrawRunningAwayIdle()
{
  owner->display.drawBitmap(100 - 45, 110, img_DaisyHog_RunningAway_Idle, 95, 55, getColor_fg());
}

void DaisyHog::DrawRunningAwayWalking(int idleIdx, int xOffset)
{
  owner->display.drawBitmap(xOffset + 100 - 45, 110, idleIdx % 2 == 0 ? img_DaisyHog_RunningAway_Walking1 : img_DaisyHog_RunningAway_Walking2, 95, 55, getColor_fg());
}

/*# ==== MUGSNAKE ====
       _____y
      / _*_/
     / /     
 ___| |__ _
 | M O N | \
 | D A Y |  |
 |_______|_/
#*/

void MugSnake::DrawStrokingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, isStrokingLeftSide ? img_MugSnake_BeingPet1 : img_MugSnake_BeingPet2, 72, 72, getColor_fg());
}
void MugSnake::DrawEatingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Eating1 : img_MugSnake_Eating2, 72, 72, getColor_fg());
}
void MugSnake::DrawSleepyPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, img_MugSnake_Hungry, 72, 72, getColor_fg()); // TODO: bespoke mugsnake animation for sleepy
}
void MugSnake::DrawAsleepPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Sleeping1 : img_MugSnake_Sleeping2, 72, 72, getColor_fg());
}
void MugSnake::DrawStarvingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, img_MugSnake_VeryHungry, 72, 72, getColor_fg());
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 97, idleIdx % 2 == 0 ? img_Emote_Hungry1 : img_Emote_Hungry2, 28, 19, getColor_fg());
  if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 95, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void MugSnake::DrawHungryPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, img_MugSnake_Hungry, 72, 72, getColor_fg());
  if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 95, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void MugSnake::DrawSadPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Sulking1 : img_MugSnake_Sulking2, 72, 72, getColor_fg());
  if (owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 95, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void MugSnake::DrawTwitchAnimationPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Rocking1 : img_MugSnake_Rocking2, 72, 72, getColor_fg());
  if (owner->getHappyTier() >= HappyTier::Blissful)
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 119, 115, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
}
void MugSnake::DrawIdlePose(int idleIdx, bool isAnimating)
{
  if (owner->getHappyTier() >= HappyTier::Happy && owner->currentTime.Hour % 2 == 0 && owner->currentTime.Minute >= 20 && owner->currentTime.Minute <= 40)
  {
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleAnimIdx % 2 == 0 ? img_MugSnake_TippedOverIdle1 : img_MugSnake_TippedOverIdle2, 72, 72, getColor_fg());
    if (!isAnimating || owner->getHappyTier() < HappyTier::Blissful)
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 120, 130, idleAnimIdx % 2 == 0 ? img_Emote_Music1 : img_Emote_Music2, 28, 19, getColor_fg());
    else
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 120, 130, idleAnimIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
  }
  else
  {
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Idle1 : img_MugSnake_Idle2, 72, 72, getColor_fg());
    if (isPeriodicAnim && owner->getHappyTier() >= HappyTier::Blissful)
      owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 95, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
  }
}

void MugSnake::DrawWalkingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 97, idleIdx % 2 == 0 ? img_MugSnake_Walking1 : img_MugSnake_Walking2, 72, 72, getColor_fg());
}

void MugSnake::DrawPosingQuestionPose(int idleIdx, bool isAnimating)
{
  // TODO: bespoke version of animation
  owner->display.drawBitmap(100 - 36, 97, idleAnimIdx % 2 == 0 ? img_MugSnake_TippedOverIdle1 : img_MugSnake_TippedOverIdle2, 72, 72, getColor_fg());
}
void MugSnake::DrawCloseUpFrame(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(0, 0, idleIdx % 2 == 0 ? img_MugSnake_CloseUp_Happy1 : img_MugSnake_CloseUp_Happy2, 200, 200, getColor_fg());
}

void MugSnake::DrawRunningAwayIdle()
{
  owner->display.drawBitmap(100 - 45, 97, img_MugSnake_RunningAway_Idle, 95, 75, getColor_fg());
}

void MugSnake::DrawRunningAwayWalking(int idleIdx, int xOffset)
{
  // TODO: mugsnake version of animation
  owner->display.drawBitmap(xOffset + 100 - 45, 97, idleIdx % 2 == 0 ? img_MugSnake_RunningAway_Walk1 : img_MugSnake_RunningAway_Walk2, 95, 75, getColor_fg());
}

/*# ==== DEERSLUG ====
                 0
           \ ___/
    ______/ \    \___
   /  0   0     \____\
  | o  . 0      _____/
  \____________/
#*/

void DeerSlug::DrawStrokingPose(int idleIdx, bool isAnimating)
{
  // TODO: DeerSlug version of animation
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, isStrokingLeftSide ? img_DeerSlug_BeingStroked1 : img_DeerSlug_BeingStroked2, 72, 55, getColor_fg());
}
void DeerSlug::DrawEatingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Eating1 : img_DeerSlug_Idle1, 72, 55, getColor_fg());
}
void DeerSlug::DrawSleepyPose(int idleIdx, bool isAnimating)
{
  // TODO: bespoke version of animation
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DeerSlug_Sad1, 72, 55, getColor_fg()); 
}
void DeerSlug::DrawAsleepPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Sleeping1 : img_DeerSlug_Sleeping2, 72, 55, getColor_fg());
}
void DeerSlug::DrawStarvingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DeerSlug_Starving, 72, 55, getColor_fg());
    if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 100, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DeerSlug::DrawHungryPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DeerSlug_Hungry1, 72, 55, getColor_fg());
    if (owner->getHappyTier() == HappyTier::Sad && owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 100, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DeerSlug::DrawSadPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, img_DeerSlug_Sad1, 72, 55, getColor_fg());
  if (owner->isUnhappinessIncreasing())
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36 + 25, 100, idleIdx % 2 == 0 ? img_Emote_Stormcloud1 : img_Emote_Stormcloud2, 28, 28, getColor_fg());
}
void DeerSlug::DrawTwitchAnimationPose(int idleIdx, bool isAnimating)
{
  // TODO: bespoke version of animation
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Eating1 : img_DeerSlug_Idle1, 72, 55, getColor_fg());
  if (owner->getHappyTier() >= HappyTier::Blissful)
    owner->display.drawBitmap(owner->getPlaymateXOffset() + 119, 115, idleIdx % 2 == 0 ? img_Emote_Hearts1 : img_Emote_Hearts2, 28, 19, getColor_fg());
}
void DeerSlug::DrawIdlePose(int idleIdx, bool isAnimating)
{
  // TODO: Special idles
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Idle1 : img_DeerSlug_Idle2, 72, 55, getColor_fg());
}

void DeerSlug::DrawWalkingPose(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(owner->getPlaymateXOffset() + 100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Walking1 : img_DeerSlug_Walking2, 77, 55, getColor_fg());
}

void DeerSlug::DrawPosingQuestionPose(int idleIdx, bool isAnimating)
{
  // TODO: bespoke version of animation
  owner->display.drawBitmap(100 - 36, 110, idleIdx % 2 == 0 ? img_DeerSlug_Idle1 : img_DeerSlug_Idle2, 72, 55, getColor_fg());
}
void DeerSlug::DrawCloseUpFrame(int idleIdx, bool isAnimating)
{
  owner->display.drawBitmap(0, 0, idleIdx % 2 == 0 ? img_DeerSlug_CloseUpHappy1 : img_DeerSlug_CloseUpHappy2, 200, 200, getColor_fg());
}

void DeerSlug::DrawRunningAwayIdle()
{
  // Babies can't run away
}

void DeerSlug::DrawRunningAwayWalking(int idleIdx, int xOffset)
{
  // Babies can't run away
}