#define KP_MOD 1
#define MAX_STRINGS 12
#include "chords.h"

// Special thanks to Matthew Rahtz - http://amid.fish/karplus-strong/
int oldPitch = 40;
uint8_t oldStrings = 0;
int oldmodValue = 127;
uint8_t strings = 0;


#ifdef KP_MOD
AudioSynthKarplusStrongMod  stringArray[MAX_STRINGS];
#else
AudioSynthKarplusStrong  stringArray[MAX_STRINGS];
#endif
AudioMixer4              mixer[4];
AudioMixer4              sumMix;

AudioConnection          patchCord0(stringArray[0], 0, mixer[0], 0);
AudioConnection          patchCord1(stringArray[1], 0, mixer[0], 1);
AudioConnection          patchCord2(stringArray[2], 0, mixer[0], 2);
AudioConnection          patchCord3(stringArray[3], 0, mixer[0], 3);

AudioConnection          patchCord4(stringArray[4], 0, mixer[1], 0);
AudioConnection          patchCord5(stringArray[5], 0, mixer[1], 1);
AudioConnection          patchCord6(stringArray[6], 0, mixer[1], 2);
AudioConnection          patchCord7(stringArray[7], 0, mixer[1], 3);

AudioConnection          patchCord8(stringArray[8], 0, mixer[2], 0);
AudioConnection          patchCord9(stringArray[9], 0, mixer[2], 1);
AudioConnection          patchCord10(stringArray[10], 0, mixer[2], 2);
AudioConnection          patchCord11(stringArray[11], 0, mixer[2], 3);

AudioConnection          patchCordSubMix0(mixer[0], 0, sumMix, 0);
AudioConnection          patchCordSubMix1(mixer[1], 0, sumMix, 1);
AudioConnection          patchCordSubMix2(mixer[2], 0, sumMix, 2);
AudioConnection          patchCordSubMix3(mixer[3], 0, sumMix, 3);

AudioConnection          patchCordOutLeft(sumMix, 0, audioOut, 0);
AudioConnection          patchCordOutRight(sumMix, 0, audioOut, 1);


#ifdef KP_MOD
void setDecayVal(uint16_t modValue)
{
  uint16_t value = 25000 + ((modValue) << 6) ; //need an offset, otherwise too severe
  for (int i = 0; i < MAX_STRINGS; i++)
  {
    stringArray[i].setDecayTime(value);
  }
  Serial.println(value);
  return;
}
#endif

void stringsSetup()
{
  for (int i; i < 4; i++)
  {
    mixer[i].gain(i, 0.13f);
    sumMix.gain(i, .1f);
  }
#ifdef KP_MOD
  setDecayVal(90);
#endif
}

void stringPlayer(int pitch, uint16_t velIn)
{

#ifdef KP_MOD
  uint16_t velocity = (velIn + 1) << 9;
#else
  float velocity = ((float)(velIn)) / 128.0;
#endif
  if (pitch == oldPitch)
  {
    strings = oldStrings;
  }
  else
  {
    if (strings >= MAX_STRINGS - 1)
    {
      strings = 0;
    }
    else
    {
      ++strings;
    }
  }
  stringArray[strings].noteOn(MidiNotesHZ[pitch], velocity);
  oldStrings = strings;
  oldPitch = pitch;

  Serial.print(pitch);
  Serial.print(" string# ");
  Serial.println(strings);
  Serial.print("Velocity ");
  Serial.print(velocity);
  Serial.println(" ");

  return;
}
