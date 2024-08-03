#pragma once
#include "Watchy_Base.h"

class Watchytchi;
class SpeciesBase;

class IdleScene
{
  public:
    Watchytchi* owner;
    SpeciesBase* critter;
    int getColor_fg()
    {
      return invertColors ? GxEPD_WHITE : GxEPD_BLACK;
    }

    void Begin();
    virtual void Tick();
    virtual void DrawBG(int idleIdx) = 0;
    virtual void DrawFG(int idleIdx) = 0;
    virtual bool IsEligible() = 0;
  private:
    float GetDurationLowRange();
    float GetDurationHighRange();
};

class IdleScene_Default : public IdleScene
{
  void Tick() {IdleScene::Tick();};
  void DrawBG(int idleIdx) override;
  void DrawFG(int idleIdx) override;
  bool IsEligible() {return true;};
};

// TODO: Dream, floor activity, ball, etc.

