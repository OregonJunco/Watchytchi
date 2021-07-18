public enum CState {IdleWander, PursueFood, Sleep}

public class Creature
{
  /*# Brain State #*/
  public CState state;

  /*# Locomotion State #*/
  float xPos, yPos;
  float desiredX = width;
  int faceDirection = 0;
  float speed = 20f;
  PVector size = new PVector(0f, 0f);
  PVector awakeDurationRange = new PVector (40f, 70);
  PVector sleepDurationRange = new PVector (90f, 120f);
  float sleepTimer;
  

  /*# Animation State #*/
  int frameIdx = 0;
  float frameTicker;
  Animation currentAnim;


  void SetAnim(Animation newAnim)
  {
    if (newAnim != currentAnim)
    {
      frameIdx = 0;
      frameTicker = 0f;
      currentAnim = newAnim;
    }
  }

  Creature(float xIn, float yIn)
  {
    xPos = xIn;
    yPos = yIn;
    
    for (int i = 0; i < idleAnim.frames.length; i++)
    {
      size.x = max(size.x, idleAnim.frames[i].width);
      size.y = max(size.y, idleAnim.frames[i].height);
    }

    sleepTimer = random(awakeDurationRange.x, awakeDurationRange.y);
    state = CState.IdleWander;
  }
  
  void Tick(float dt)
  {
    sleepTimer -= dt;
    if (sleepTimer < 0)
    {
      if (state == CState.Sleep)
      {
        sleepTimer = random(awakeDurationRange.x, awakeDurationRange.y);
        state = CState.IdleWander;
      }
      else
      {
        sleepTimer = random(sleepDurationRange.x, sleepDurationRange.y);
        state = CState.Sleep;
        SetAnim(sleepAnim);
      }
    }

    if (state != CState.Sleep && activeFoods.size() != 0 && state != CState.PursueFood)
      state = CState.PursueFood;
    else if (activeFoods.size() == 0 && state == CState.PursueFood)
    {
      state = CState.IdleWander;
      faceDirection = 0;
    }
      
    if (state == CState.IdleWander)
    {
      SetAnim(stepAnim);

      // Basic ping ponging walk behavior
      if (faceDirection == 0 || (faceDirection == -1 && xPos < 0))
        desiredX = width + 20;
      else if (faceDirection == 1 && xPos > width)
        desiredX = -20;
    }
    else if (state == CState.PursueFood)
    {
      if (activeFoods.get(0).IsFalling())
        desiredX = 60;
      else
        desiredX = 110f;

      if (IsAtTarget())
      {
        faceDirection = 1;
        if (activeFoods.get(0).IsFalling())
          SetAnim(idleAnim);
        else
        {
          SetAnim(eatAnim);
          activeFoods.get(0).TickEat(dt);
        }
      }
    }

    float deltaToTarget = desiredX - xPos;
    if (!IsAtTarget() && state != CState.Sleep)
    {
      faceDirection = deltaToTarget > 0 ? 1 : -1;
      float walkDelta = min(abs(deltaToTarget), speed * dt) * faceDirection;
      xPos += walkDelta;
      SetAnim(stepAnim);
    }
  }

  boolean IsAtTarget()
  {
    return abs(desiredX - xPos) <= 0.000001f;
  }

  void Draw(float dt)
  {
    frameTicker += dt;
    while (frameTicker > currentAnim.frameInterval)
    {
      frameIdx = (frameIdx + 1) % currentAnim.frames.length;
      frameTicker -= currentAnim.frameInterval;
    }

    pushMatrix();
    translate(xPos - size.x / 2f * faceDirection, yPos - size.y);
    scale(faceDirection, 1.0);
    image(currentAnim.frames[frameIdx], 0, 0);
    popMatrix();

    if (doDebug)
    {
      fill(255, 0, 255);
      square(xPos, yPos, 2);
    }
  }
}