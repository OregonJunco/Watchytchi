import processing.sound.*;

/*# Constants #*/
String savePath = "save/SaveData.json";
float maxHunger = 100;
int drawFrameRate = 16;
int tickFrameRate = 2;
float gravitySpeed = 75f;
float floorY = 200f-40f;
boolean doDebug = false;
final char key_cursor = 'z';
final char key_select = 'x';
final char key_back = 'c';


/*# Assets #*/
PFont font_20;
AnimationData stepAnim;
AnimationData idleAnim;
AnimationData eatAnim;
AnimationData sleepAnim;
AnimationData poopAnim;
SoundFile sfx_VibeCursor;
SoundFile sfx_VibeFail;
SoundFile sfx_VibeSelect;
FoodData foodData_Berry;
PImage sunSprite;
PImage moonSprite;
PImage[] cloudSprites;
PImage statusMenuBackground;
JSONArray velveteenData;

/*# Turtles #*/
Creature creature;
ArrayList<Turtle> turtles = new ArrayList<Turtle>();
ArrayList<UIScreen> uiStack = new ArrayList<UIScreen>();
ArrayList<FoodInstance> activeFoods = new ArrayList<FoodInstance>();

/*# UI #*/
MainMenuUI mainMenu;
StatusScreen statusScreen;
CleaningScreen cleaningScreen;
StorybookScreen storybookScreen;

/*# State #*/
PoopManager poopManager;
JSONObject saveJson;
boolean areLightsOn = true;
float hunger = maxHunger;
float age = 0;
long lastUpdateTs;
long frameNumber = 0;
int velveteenIndex = 0;

void RestoreFromDisk()
{
  try {
    saveJson = loadJSONObject(savePath);
  }
  // HACK: we'll get an exception if the file doesn't exist. There's probably a better way to check if it exists
  catch (Exception e) {
    // If file doesn't exist, create it
    PrintWriter writer = createWriter(savePath);
    writer.println("{}");
    writer.flush();
    saveJson = loadJSONObject(savePath);
    return;
  }
    
  age = saveJson.getFloat("age", 0f);
  hunger = saveJson.getFloat("hunger", maxHunger);
  areLightsOn = saveJson.getBoolean("areLightsOn", true);
  creature.poopTimer = saveJson.getFloat("poopTimer", 0f);
  creature.isTickingPoopTimer = saveJson.getBoolean("isTickingPoopTimer", false);
  poopManager.RestoreFromDisk(saveJson);
  velveteenIndex = saveJson.getInt("velveteenIndex", 0);

  velveteenData = loadJSONArray("VelveteenData.json");
  for (int i = 0; i < velveteenData.size(); i++)
  {
    println(velveteenData.getString(i));
  }
}

void SaveToDisk()
{
  saveJson.setFloat("age", age);
  saveJson.setFloat("hunger", hunger);
  saveJson.setBoolean("areLightsOn", areLightsOn);
  saveJson.setFloat("poopTimer", creature.poopTimer);
  saveJson.setBoolean("isTickingPoopTimer", creature.isTickingPoopTimer);
  poopManager.SaveToDisk(saveJson);
  saveJson.setInt("velveteenIndex", velveteenIndex);
  saveJSONObject(saveJson, "save/SaveData.json");
}

void setup()
{
  // Initialize drawing parameters
  frameRate(drawFrameRate);
  noStroke();
  size(200, 200);

  // Load resources
  font_20 = loadFont("Cambria-20.vlw");
  sfx_VibeCursor = new SoundFile(this, "sfx_VibeCursor.wav");
  sfx_VibeFail = new SoundFile(this, "sfx_VibeFail.wav");
  sfx_VibeSelect = new SoundFile(this, "sfx_VibeSelect.wav");
  stepAnim = new AnimationData("DaisyHog_Step", 2, 0);
  idleAnim = new AnimationData("DaisyHog_Idle", 2, 0);
  eatAnim = new AnimationData("DaisyHog_Eat", 2, 0);
  sleepAnim = new AnimationData("DaisyHog_Sleep", 2, 1);
  poopAnim = new AnimationData("Poop_", 2, 0);
  sunSprite = loadImage("Sun_Dithered.png");
  moonSprite = loadImage("Moon_Dithered.png");
  cloudSprites = LoadImageArray("Cloud", 3);
  statusMenuBackground = loadImage("StatusBackground.png");

  // Initialize UI:
  mainMenu = new MainMenuUI();
  statusScreen = new StatusScreen();
  cleaningScreen = new CleaningScreen();
  storybookScreen = new StorybookScreen();
  uiStack.add(mainMenu);

  // Load resources (food)
  foodData_Berry = new FoodData("FoodBerry_Stage", 7, 20f);

  // Initialize creature
  creature = new Creature(new PVector(width / 2f, floorY));
  
  // Initialize Environment
  turtles.add(new HeavenlyBody());
  for (int i = 0; i < 2; i++)
    turtles.add(new Cloud());
  poopManager = new PoopManager();

  // Load save data
  RestoreFromDisk();
  
  lastUpdateTs = System.currentTimeMillis();
}

void exit()
{
  SaveToDisk();
  super.exit();
}

void draw()
{
  background(255, 255, 255);
  
  int tickInterval = drawFrameRate / tickFrameRate;
  boolean isTickFrame = frameNumber % tickInterval == 0; 
  frameNumber++;
  
  float dt = 0f;
  // Calculate deltaTime
  if (isTickFrame)
  {
    long currentTs = System.currentTimeMillis();
    dt = (currentTs - lastUpdateTs) / 1000f;
    lastUpdateTs = currentTs;
    
    hunger = constrain(hunger - dt / 4f, 0, maxHunger);
    age += dt / 60f;

    for (int i = 0; i < uiStack.size(); i++)
    {
      uiStack.get(i).Tick(dt);
    }
    creature.Tick(dt);
    for (int i = 0; i < activeFoods.size(); i++)
    {
      activeFoods.get(i).Tick(dt);
    }
    for (int i = 0; i < turtles.size(); i++)
    {
      turtles.get(i).Tick(dt);
    }
  }
  
  for (int i = 0; i < activeFoods.size(); i++)
  {
    activeFoods.get(i).Draw();
  }
  for (int i = 0; i < turtles.size(); i++)
  {
    turtles.get(i).Draw();
  }
  creature.Draw();
  
  // Draw UI:
  int uiDrawStartIndex = 0;
  for (int i = uiStack.size() - 1; i >= 0; i--)
  {
    if (uiStack.get(i).BlocksDrawingBelow())
    {
      uiDrawStartIndex = i;
      break;
    }
  }
  
  for (int i = uiDrawStartIndex; i < uiStack.size(); i++)
  {
    uiStack.get(i).Draw();
  }

  if (!areLightsOn)
    filter(INVERT);
}

void keyPressed()
{
  for (int i = uiStack.size() - 1; i >= 0; i--)
  {
    if (uiStack.get(i).HandleInput())
      break;
  }
}

PImage[] LoadImageArray(String baseString, int numVariants)
{
  return LoadImageArray(baseString, numVariants, 1);
}

PImage[] LoadImageArray(String baseString, int numVariants, int startIdx)
{
  PImage[] output = new PImage[numVariants];
  for (int i = 0; i < numVariants; i++)
      output[i] = loadImage(baseString + (i + startIdx) + ".png");
  return output;
}


class AnimationData
{
  public int numFrameSkips;
  public PImage[] frames;

  AnimationData(String baseString, int numFrames, int numFrameSkipsIn)
  {
    frames = LoadImageArray(baseString, numFrames);
    numFrameSkips = numFrameSkipsIn;
  }

  AnimationData(PImage[] framesIn, int numFrameSkipsIn)
  {
    frames = framesIn;
    numFrameSkips = numFrameSkipsIn;
  }
}
