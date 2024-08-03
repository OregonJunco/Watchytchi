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
    virtual bool OffsetCreatureToLeft() {return false;}
  protected:
    int GetTimeInScene();
    float GetProgressT();
    int GetFrameByProgress(int maxFrames);
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
  bool OffsetCreatureToLeft() {return false;}
};

class IdleScene_FloorActivity : public IdleScene
{
  public:
    IdleScene_FloorActivity(std::vector<const unsigned char*>* frames);

    void Tick() {IdleScene::Tick();};
    void DrawBG(int idleIdx) override;
    void DrawFG(int idleIdx) override;
    bool IsEligible() {return true;};
    bool OffsetCreatureToLeft() {return true;}
  private:
    std::vector<const unsigned char*>* frames;
};

// TODO: Dream, floor activity, ball, etc.

