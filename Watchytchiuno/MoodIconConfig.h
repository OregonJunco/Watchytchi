#pragma once

class MoodIconConfig
{
  public:
    const unsigned char* img_AdultHappy;
    const unsigned char* img_AdultSad;
    const unsigned char* img_BabyHappy;
    const unsigned char* img_BabySad;

    MoodIconConfig(const unsigned char* adultHappy, const unsigned char* adultSad, 
      const unsigned char* babyHappy, const unsigned char* babySad)
    {
      img_AdultHappy = adultHappy;
      img_AdultSad = adultSad;
      img_BabyHappy = babyHappy;
      img_BabySad = babySad;
    }

  static MoodIconConfig food;
  static MoodIconConfig stroke;
  static MoodIconConfig walk;
  static MoodIconConfig poop;
  static MoodIconConfig sleep;
  static MoodIconConfig playmate;
  static MoodIconConfig hotSprings;
  static MoodIconConfig read;
};


// static MoodIconConfig moodIconConfig_Food = MoodIconConfig(img_MoodleIcon_FoodHappy_Adult, img_MoodleIcon_FoodSad_Adult,
//   img_MoodleIcon_FoodHappy_Baby, img_MoodleIcon_FoodSad_Baby);

// static MoodIconConfig moodIconConfig_Stroke = MoodIconConfig(img_MoodleIcon_StrokeHappy_Adult, img_MoodleIcon_Invalid,
//   img_MoodleIcon_StrokeHappy_Baby, img_MoodleIcon_Invalid);

// static MoodIconConfig moodIconConfig_Walk = MoodIconConfig(img_MoodleIcon_WalkHappy_Adult, img_MoodleIcon_Invalid,
//   img_MoodleIcon_WalkHappy_Baby, img_MoodleIcon_Invalid);

// static MoodIconConfig moodIconConfig_Poop = MoodIconConfig(img_MoodleIcon_PoopHappy_Adult, img_MoodleIcon_PoopSad_Adult,
//   img_MoodleIcon_PoopHappy_Baby, img_MoodleIcon_PoopSad_Baby);

// static MoodIconConfig moodIconConfig_Sleep = MoodIconConfig(img_MoodleIcon_SleepHappy_Adult, img_MoodleIcon_SleepSad_Adult,
//   img_MoodleIcon_SleepHappy_Baby, img_MoodleIcon_SleepSad_Baby);

// static MoodIconConfig moodIconConfig_Playmate = MoodIconConfig(img_MoodleIcon_PlaymateHappy_Adult, img_MoodleIcon_Invalid,
//   img_MoodleIcon_PlaymateHappy_Baby, img_MoodleIcon_Invalid);

// static MoodIconConfig moodIconConfig_HotSprings = MoodIconConfig(img_MoodleIcon_HotSpringsHappy_Adult, img_MoodleIcon_Invalid,
//   img_MoodleIcon_HotSpringsHappy_Baby, img_MoodleIcon_Invalid);

// static MoodIconConfig moodIconConfig_Read = MoodIconConfig(img_MoodleIcon_ReadHappy_Adult, img_MoodleIcon_Invalid,
//   img_MoodleIcon_ReadHappy_Baby, img_MoodleIcon_Invalid);
