#pragma once
#include "Watchy_Base.h"
#include "ImageAssets.h"
class MoodIconConfig;

enum TimeOfDay {Daytime, Dusk, LateNight};
enum HappyTier {Sad, Neutral, Happy, Blissful};
enum ContextAction {NoContextActionAvailable, LightsOff, Clean, TriggerAlert};

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
        ContextAction getActiveContextAction();
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
        void clearNonUIBackground();
        void clearScreen();
        void drawUIButton(int idx, bool quickCursorUpdate);
        void drawAllUIButtons();
        void drawBgEnvironment();
        void drawWeather();
        bool tryDrawMoodle(int& idx, const MoodIconConfig* iconConfig, float happyLevel);
        void drawIdleCreature(bool isAnimating);
        void drawEatAnim();
        void drawPlaymate(int idleIdx, int xOffset = 0, int yOffset = 0);
        void drawAgeFlower();
        void drawDebugClock();
        void drawPoop();

        /*## Draw functions (Schedulable Alerts) ##*/
        void executeHWYDResponse();

        /*# State-based handlers: #*/ 
        bool dummy_handleButtonPress(uint64_t wakeupBit);
        void baseMenu_draw();
        bool baseMenu_handleButtonPress(uint64_t wakeupBit);
        void statusCheck_draw();
        bool statusCheck_handleButtonPress(uint64_t wakeupBit);
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
        void settings_draw();
        bool settings_handleButtonPress(uint64_t wakeupBit);
        void hotSpringsTimer_draw();
        bool hotSpringsTimer_handleButtonPress(uint64_t wakeupBit);
        void reading_draw();
        bool reading_handleButtonPress(uint64_t wakeupBit);
        void intro_draw();
        bool intro_handleButtonPress(uint64_t wakeupBit);

        
        typedef void (Watchytchi::*DrawFunc)();
        DrawFunc drawFuncsByState[(int)GameState::CNT] = {
            &Watchytchi::baseMenu_draw, // BASEMENU
            &Watchytchi::statusCheck_draw, // STATUSCHECK
            &Watchytchi::eating_draw, // EATING
            &Watchytchi::alertInteraction_draw, // ALERT INTERACTION
            &Watchytchi::stroking_draw, // STROKING
            &Watchytchi::howWasYourDay_draw, // HOW WAS YOUR DAY
            &Watchytchi::ending_draw, // ENDING
            &Watchytchi::sharedWalk_draw, // SHARED WALK
            &Watchytchi::settings_draw, // SETTINGS
            &Watchytchi::hotSpringsTimer_draw, // HOT SPRINGS TIMER
            &Watchytchi::reading_draw, // READING
            &Watchytchi::intro_draw // INTRO
        };

        typedef bool (Watchytchi::*ButtonFunc)(long long unsigned int);
        ButtonFunc handleButtonFuncsByState[(int)GameState::CNT] = {
            &Watchytchi::baseMenu_handleButtonPress, // BASEMENU
            &Watchytchi::statusCheck_handleButtonPress, // STATUSCHECK
            &Watchytchi::dummy_handleButtonPress, // EATING
            &Watchytchi::dummy_handleButtonPress, // ALERT INTERACTION
            &Watchytchi::stroking_handleButtonPress, // STROKING
            &Watchytchi::howWasYourDay_handleButtonPress, // HOW WAS YOUR DAY
            &Watchytchi::ending_handleButtonPress, // ENDING
            &Watchytchi::sharedWalk_handleButtonPress, // SHARED WALK
            &Watchytchi::settings_handleButtonPress, // SETTINGS
            &Watchytchi::hotSpringsTimer_handleButtonPress, // HOT SPRINGS TIMER
            &Watchytchi::reading_handleButtonPress, // READING
            &Watchytchi::intro_handleButtonPress // INTRO
        };
};
