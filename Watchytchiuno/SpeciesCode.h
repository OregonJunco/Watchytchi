#pragma once
#include "Watchy_Base.h"
#include "Watchy_Watchytchi.h"

class SpeciesBase
{

  public:
    Watchytchi* owner;
    int getColor_fg()
    {
      return invertColors ? GxEPD_WHITE : GxEPD_BLACK;
    }

    virtual void DrawStrokingPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawEatingPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawSleepyPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawAsleepPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawStarvingPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawHungryPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawSadPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawTwitchAnimationPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawIdlePose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawWalkingPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawHotSpringsPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawReadingPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawPosingQuestionPose(int idleIdx, bool isAnimating) = 0;
    virtual void DrawCloseUpFrame(int idleIdx, bool isAnimating) = 0;
    virtual void DrawRunningAwayIdle() = 0;
    virtual void DrawRunningAwayWalking(int idleIdx, int xOffset) = 0;
    virtual void DrawGoodEndPackedBags(int idleIdx) = 0;
    virtual void DrawGoodEndHike(int idleIdx) = 0;
    virtual void DrawGoodEndHug(int idleIdx) = 0;
    virtual void DrawGoodEndIntroBonfire(int idleIdx) = 0;
    virtual void DrawGoodEndOutroBonfire(int idleIdx) = 0;
};

class DaisyHog : public SpeciesBase
{
  void DrawStrokingPose(int idleIdx, bool isAnimating) override;
  void DrawEatingPose(int idleIdx, bool isAnimating) override;
  void DrawSleepyPose(int idleIdx, bool isAnimating) override;
  void DrawAsleepPose(int idleIdx, bool isAnimating) override;
  void DrawStarvingPose(int idleIdx, bool isAnimating) override;
  void DrawHungryPose(int idleIdx, bool isAnimating) override;
  void DrawSadPose(int idleIdx, bool isAnimating) override;
  void DrawTwitchAnimationPose(int idleIdx, bool isAnimating) override;
  void DrawIdlePose(int idleIdx, bool isAnimating) override;
  void DrawWalkingPose(int idleIdx, bool isAnimating) override;
  void DrawHotSpringsPose(int idleIdx, bool isAnimating) override;
  void DrawReadingPose(int idleIdx, bool isAnimating) override;
  void DrawPosingQuestionPose(int idleIdx, bool isAnimating) override;
  void DrawCloseUpFrame(int idleIdx, bool isAnimating) override;
  void DrawRunningAwayIdle() override;
  void DrawRunningAwayWalking(int idleIdx, int xOffset) override;
  void DrawGoodEndPackedBags(int idleIdx) override;
  void DrawGoodEndHike(int idleIdx) override;
  void DrawGoodEndHug(int idleIdx) override;
  void DrawGoodEndIntroBonfire(int idleIdx) override;
  void DrawGoodEndOutroBonfire(int idleIdx) override;
};

class MugSnake : public SpeciesBase
{
  void DrawStrokingPose(int idleIdx, bool isAnimating) override;
  void DrawEatingPose(int idleIdx, bool isAnimating) override;
  void DrawSleepyPose(int idleIdx, bool isAnimating) override;
  void DrawAsleepPose(int idleIdx, bool isAnimating) override;
  void DrawStarvingPose(int idleIdx, bool isAnimating) override;
  void DrawHungryPose(int idleIdx, bool isAnimating) override;
  void DrawSadPose(int idleIdx, bool isAnimating) override;
  void DrawTwitchAnimationPose(int idleIdx, bool isAnimating) override;
  void DrawIdlePose(int idleIdx, bool isAnimating) override;
  void DrawWalkingPose(int idleIdx, bool isAnimating) override;
  void DrawHotSpringsPose(int idleIdx, bool isAnimating) override;
  void DrawReadingPose(int idleIdx, bool isAnimating) override;
  void DrawPosingQuestionPose(int idleIdx, bool isAnimating) override;
  void DrawCloseUpFrame(int idleIdx, bool isAnimating) override;
  void DrawRunningAwayIdle() override;
  void DrawRunningAwayWalking(int idleIdx, int xOffset) override;
  void DrawGoodEndPackedBags(int idleIdx) override;
  void DrawGoodEndHike(int idleIdx) override;
  void DrawGoodEndHug(int idleIdx) override;
  void DrawGoodEndIntroBonfire(int idleIdx) override;
  void DrawGoodEndOutroBonfire(int idleIdx) override;
};

class DeerSlug : public SpeciesBase
{
  void DrawStrokingPose(int idleIdx, bool isAnimating) override;
  void DrawEatingPose(int idleIdx, bool isAnimating) override;
  void DrawSleepyPose(int idleIdx, bool isAnimating) override;
  void DrawAsleepPose(int idleIdx, bool isAnimating) override;
  void DrawStarvingPose(int idleIdx, bool isAnimating) override;
  void DrawHungryPose(int idleIdx, bool isAnimating) override;
  void DrawSadPose(int idleIdx, bool isAnimating) override;
  void DrawTwitchAnimationPose(int idleIdx, bool isAnimating) override;
  void DrawIdlePose(int idleIdx, bool isAnimating) override;
  void DrawWalkingPose(int idleIdx, bool isAnimating) override;
  void DrawHotSpringsPose(int idleIdx, bool isAnimating) override;
  void DrawReadingPose(int idleIdx, bool isAnimating) override;
  void DrawPosingQuestionPose(int idleIdx, bool isAnimating) override;
  void DrawCloseUpFrame(int idleIdx, bool isAnimating) override;
  void DrawRunningAwayIdle() override;
  void DrawRunningAwayWalking(int idleIdx, int xOffset) override;
  void DrawGoodEndPackedBags(int idleIdx) override;
  void DrawGoodEndHike(int idleIdx) override;
  void DrawGoodEndHug(int idleIdx) override;
  void DrawGoodEndIntroBonfire(int idleIdx) override;
  void DrawGoodEndOutroBonfire(int idleIdx) override;

};