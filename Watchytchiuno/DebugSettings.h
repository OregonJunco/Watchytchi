#define VERBOSE_LOGGING_ENABLED 0 // 1 to enable all DBGPrints, 0 to disable for performance
#define DISABLE_MAX_TIMESTAMP 0 // 1 to disable time delta clamping, allowing you to fast forward
#define FORCED_NEXT_EVENT -1 // -1 to disable, 0+ to force a schedulable event (matching the SchedulableEvent enum)
#define FORCED_ACTIVE_PLAYMATE -1 // -1 to disable, 0+ to force an active playmate (matching the PlaymateSpecies enum)
#define FORCED_ENDING -1 // -1 to disable, 0 to force good ending, 1 to force bad ending
#define SHORT_IDLESCENES 0 // 0 for normal length idle scenes, 1 to reduce them to 30 seconds long