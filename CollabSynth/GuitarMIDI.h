#define MAX_STRINGS 12
#define PITCHBEND_RANGE -(1.0/8192.0)  //should be 8192 >> or << by no more than say 5
#include "chords.h" //was in original example, might be useful

// Special thanks to Matthew Rahtz - http://amid.fish/karplus-strong/

AudioSynthKarplusStrongMod  stringArray[MAX_STRINGS];

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

void scalerSetup()//might be a better way to do this, but right now it is useful to be able to do it this way
{// each octave gets a different scaler. This scaler is also used in the actual string synth
  for (int i = 0; i <= 11; i++)
  {
    scalerMidiNote[i] = 1<<5;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  } 
  for (int i = 12; i <= 23; i++)
  {
    scalerMidiNote[i] = 1<<4;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  } 
  for (int i = 24; i <= 35; i++)
  {
    scalerMidiNote[i] = 1<<3;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
  for (int i = 36; i <= 47; i++)
  {
    scalerMidiNote[i] = 1<<2;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
  for (int i = 48; i <= 59; i++)
  {
    scalerMidiNote[i] = 1<<1;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
  for (int i = 60; i <= 71; i++)
  {
    scalerMidiNote[i] = 1<<0;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
    for (int i = 72; i <= 83; i++)
  {
    scalerMidiNote[i] = 1<<0;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
    for (int i = 84; i <= 95; i++)
  {
    scalerMidiNote[i] = 1<<0;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
    for (int i = 96; i <= 107; i++)
  {
    scalerMidiNote[i] = 1<<0;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
    for (int i = 108; i <= 127; i++)
  {
    scalerMidiNote[i] = 1<<0;
    midiNotesScaled[i] = MidiNotesDelay[i]/scalerMidiNote[i];
  }
  
}

void setDecayVal(uint16_t modValue)// this sets the decay time of the strings. It is currently mapped to the mod wheel, but is technically actually a sustain
{
  int32_t value = 30740 + ((modValue) << 4) ; //need an offset, otherwise too severe

  stringArray[0].setDecayTime(value);//it is currently static, but could actually be implemented differently on individual strings
  return;
}

void setPitchBend(int pitchBend)//pitchbend - currently is not optimal on bend down during decay
{
  float pitchFloat = ((float)pitchBend) * PITCHBEND_RANGE;
  float pitchResult = exp2f(pitchFloat);
  stringArray[0].setPitchBend(pitchResult);
  return;
}

void stringsSetup()//initial setup
{
  scalerSetup();
  for (int i = 0; i < 4; i++)
  {
    mixer[i].gain(i, 0.13f);
    sumMix.gain(i, .25f);
  }
  for (int i=0; i<MAX_STRINGS-1; i++)
    {
      stringArray[i].noteOn(69, i, 0);
    }
  setDecayVal(127);
  setPitchBend(0);
  return;
}

void stringPlayer(uint16_t midiNote, uint16_t velIn)//two parameters from the midi section.  
{
  uint16_t velocity = velIn << 9;
  static uint8_t strings;
    if (strings >= MAX_STRINGS - 1)
    {
      stringArray[0].noteOff();
      stringArray[0].noteOn(midiNotesScaled[midiNote], velocity, scalerMidiNote[midiNote],strings);
      strings = 0;
    }
    else
    {
      stringArray[strings++].noteOn(midiNotesScaled[midiNote], velocity, scalerMidiNote[midiNote],strings);
    }

  return;
}
