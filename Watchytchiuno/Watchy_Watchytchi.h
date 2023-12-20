#pragma once
#include "Watchy_Base.h"
#include "ImageAssets.h"

enum TimeOfDay {Daytime, Dusk, LateNight};
enum HappyTier {Sad, Neutral, Happy, Blissful};

class SpeciesBase; // Needed to prevent circular include

class Watchytchi : public WatchyBase{
    using WatchyBase::WatchyBase;
    public:
        /*# Constants #*/
        SpeciesBase* critter = nullptr;
        const int k_alertInterval = 3 * 60 * 60;
        const int k_alertExpirationWindow = 30 * 60;

        /*# Lifecycle #*/
        void drawWatchFace();
        void handleButtonPress();

        /*# Save/load #*/
        void tryLoadSaveData(bool force);
        void loadFromRTC();
        void tryWriteSaveData(bool force);
        void writeToRTC();
        void resetSaveData();

        /*# Game Logic #*/
        void tickCreatureState();
        TimeOfDay getTimeOfDay();
        TimeOfDay getTimeOfDay(const tmElements_t &tm);
        float getHappyPercent(bool shouldConstrain = true);
        HappyTier getHappyTier();
        HappyTier getHappyTier(float happyPercent);
        bool isUnhappinessIncreasing();
        bool qualifiesForBadEnd();
        bool isElectricLit();
        void chooseNewActivePlaymate(bool onlyBefriended);
        bool hasActivePlaymate();
        int getPlaymateXOffset();

        /*## Game Logic (Schedulable Events) ##*/
        bool hasActiveAlert();
        void scheduleNextAlert();
        void executeCloseUp();
        void poseHWYDQuestion();

        /*# Draw functions #*/
        void clearCreatureBackground();
        void clearScreen();
        void drawUIButton(int idx, bool quickCursorUpdate);
        void drawAllUIButtons();
        void drawBgEnvironment();
        void drawWeather();
        void drawIdleCreature(bool isAnimating);
        void drawEatAnim();
        void drawPlaymate(int idleIdx);
        void drawAgeFlower();
        void drawDebugClock();
        void drawPoop();
        void drawStatusDisplay();
        /*## Draw functions (Schedulable Alerts) ##*/
        void executeHWYDResponse();

        /*# State-based handlers: #*/ 
        bool dummy_handleButtonPress(uint64_t wakeupBit);
        void baseMenu_draw();
        bool baseMenu_handleButtonPress(uint64_t wakeupBit);
        void eating_draw();
        void alertInteraction_draw();
        void stroking_draw();
        bool stroking_handleButtonPress(uint64_t wakeupBit);
        void howWasYourDay_draw();
        bool howWasYourDay_handleButtonPress(uint64_t wakeupBit);
        void ending_draw();
        bool ending_handleButtonPress(uint64_t wakeupBit);
        void sharedWalk_draw();
        bool sharedWalk_handleButtonPress(uint64_t wakeupBit);
        void activitySelect_draw();
        bool activitySelect_handleButtonPress(uint64_t wakeupBit);
        void hotSpringsTimer_draw();
        bool hotSpringsTimer_handleButtonPress(uint64_t wakeupBit);

        
        typedef void (Watchytchi::*DrawFunc)();
        DrawFunc drawFuncsByState[(int)GameState::CNT] = {
            &Watchytchi::baseMenu_draw, // BASEMENU
            &Watchytchi::eating_draw, // EATING
            &Watchytchi::alertInteraction_draw, // ALERT INTERACTION
            &Watchytchi::stroking_draw, // STROKING
            &Watchytchi::howWasYourDay_draw, // HOW WAS YOUR DAY
            &Watchytchi::ending_draw, // ENDING
            &Watchytchi::sharedWalk_draw, // SHARED WALK
            &Watchytchi::activitySelect_draw, // ACTIVITY SELECT
            &Watchytchi::hotSpringsTimer_draw // HOT SPRINGS TIMER
        };

        typedef bool (Watchytchi::*ButtonFunc)(long long unsigned int);
        ButtonFunc handleButtonFuncsByState[(int)GameState::CNT] = {
            &Watchytchi::baseMenu_handleButtonPress, // BASEMENU
            &Watchytchi::dummy_handleButtonPress, // EATING
            &Watchytchi::dummy_handleButtonPress, // ALERT INTERACTION
            &Watchytchi::stroking_handleButtonPress, // STROKING
            &Watchytchi::howWasYourDay_handleButtonPress, // HOW WAS YOUR DAY
            &Watchytchi::ending_handleButtonPress, // ENDING
            &Watchytchi::sharedWalk_handleButtonPress, // SHARED WALK
            &Watchytchi::activitySelect_handleButtonPress, // ACTIVITY SELECT
            &Watchytchi::hotSpringsTimer_handleButtonPress // HOT SPRINGS TIMER
        };
};
