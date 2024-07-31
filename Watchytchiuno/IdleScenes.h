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

    virtual void DrawBG(int idleIdx) = 0;
    virtual void DrawFG(int idleIdx) = 0;
    virtual bool IsEligible() = 0;
};

class IdleScene_Default : public IdleScene
{
  void DrawBG(int idleIdx) override;
  void DrawFG(int idleIdx) override;
  bool IsEligible() override;
};

// TODO: Dream, floor activity, ball, etc.

