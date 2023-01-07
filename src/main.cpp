#include <M5StickCPlus.h>
#include <Preferences.h>

// A
#define BEEP_FREQUENCY 440

#define DEFAULT_BPM 100
#define MAX_BPM 300
#define MIN_BPM 10
#define PREFERENCE_NAME "m5metronome"
#define PREFERENCE_BPM_KEY "bpm"

int bpm = 0;
bool soundOn = true;
float note = 0;

Preferences preferences;

int upCount = 0;
int downCount = 0;

int getBpmChangeValueLongPress()
{
  if (M5.BtnA.pressedFor(1000))
  {
    (M5.BtnA.isPressed())
    {
      upCount++;
      if (upCount > 500)
      {
        upCount = 0;
        return 1;
      }
      return 0;
    }
  }
  else if (M5.BtnA.wasPressed())
  {
    return 1;
  }
  else if (M5.BtnB.pressedFor(1000))
  {
    (M5.BtnB.isPressed())
    {
      downCount++;
      if (downCount > 500)
      {
        downCount = 0;
        return 1;
      }
      return 0;
    }
  }
  else if (M5.BtnB.wasPressed())
  {
    return -1;
  }
  return 0;
}

int getBpmChangeValue()
{
  if (M5.BtnA.wasReleasefor(3000))
  {
    return 50;
  }
  else if (M5.BtnA.wasReleasefor(2000))
  {
    return 30;
  }
  else if (M5.BtnA.wasReleasefor(1000))
  {
    return 10;
  }
  else
  {
    if (M5.BtnA.wasPressed())
    {
      return 1;
    }
  }
  if (M5.BtnB.wasReleasefor(3000))
  {
    return -50;
  }
  else if (M5.BtnB.wasReleasefor(2000))
  {
    return -30;
  }
  else if (M5.BtnB.wasReleasefor(1000))
  {
    return -10;
  }
  else
  {
    if (M5.BtnB.wasPressed())
    {
      return -1;
    }
  }
  return 0;
}

int updateBpm(int bpm)
{
  int val = getBpmChangeValue();
  if (val == 0)
  {
    return bpm;
  }

  if (bpm + val < MIN_BPM)
  {
    bpm = MIN_BPM;
  }
  else if (bpm + val > MAX_BPM)
  {
    bpm = MAX_BPM;
  }
  else
  {
    bpm += val;
  }
  return bpm;
}

void printBpm()
{
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 20);
  M5.Lcd.println(bpm);
}

void changeBpm()
{
  int oldBpm = bpm;
  bpm = updateBpm(bpm);
  if (bpm == oldBpm)
  {
    return;
  }

  preferences.begin(PREFERENCE_NAME);
  preferences.putInt(PREFERENCE_BPM_KEY, bpm);
  preferences.end();
  printBpm();
}

void calcBpm()
{
  note = (60000 / bpm) / 4;
}

void playSound()
{
  calcBpm();
  M5.Beep.tone(BEEP_FREQUENCY, note);
  delay(note);
  M5.Beep.mute();
  delay(note * 3);
}

void setLcdRotationByAttitude()
{
  float pitch, roll, yaw;
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);
  int rotation = (pitch < 0) ? 1 : 3;

  M5.Lcd.setRotation(rotation);
}

void setup()
{
  M5.begin();
  M5.IMU.Init();
  M5.Lcd.setTextFont(7);
  M5.Lcd.setTextSize(2);
  setLcdRotationByAttitude();

  // Save BPM
  preferences.begin(PREFERENCE_NAME);
  bpm = preferences.getInt(PREFERENCE_BPM_KEY, DEFAULT_BPM);
  preferences.end();

  // Print BPM
  printBpm();
}

void loop()
{
  M5.update();

  changeBpm();
  setLcdRotationByAttitude();

  // Less than 1 second click of the power button
  if (M5.Axp.GetBtnPress() == 2)
  {
    soundOn = !soundOn;
  }

  if (soundOn)
  {
    playSound();
  }
}