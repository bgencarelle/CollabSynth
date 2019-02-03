

#include "chords.h"

// Special thanks to Matthew Rahtz - http://amid.fish/karplus-strong/
int oldPitch = 40;
int oldStrings = 0;

AudioSynthKarplusStrong  string0;
AudioSynthKarplusStrong  string1;
AudioSynthKarplusStrong  string2;
AudioSynthKarplusStrong  string3;
AudioSynthKarplusStrong  string4;
AudioSynthKarplusStrong  string5;
AudioSynthKarplusStrong  string6;
AudioSynthKarplusStrong  string7;
AudioSynthKarplusStrong  string8;
AudioSynthKarplusStrong  string9;
AudioSynthKarplusStrong  string10;
AudioSynthKarplusStrong  string11;

AudioMixer4              mixer0;
AudioMixer4              mixer1;
AudioMixer4              mixer2;
AudioMixer4              mixer3;
AudioMixer4              sumMix;


AudioConnection          patchCord0(string0, 0, mixer0, 0);
AudioConnection          patchCord1(string1, 0, mixer0, 1);
AudioConnection          patchCord2(string2, 0, mixer0, 2);
AudioConnection          patchCord3(string3, 0, mixer0, 3);

AudioConnection          patchCord4(string4, 0, mixer1, 0);
AudioConnection          patchCord5(string5, 0, mixer1, 1);
AudioConnection          patchCord6(string6, 0, mixer1, 2);
AudioConnection          patchCord7(string7, 0, mixer1, 3);


AudioConnection          patchCord8(string8, 0, mixer2, 0);
AudioConnection          patchCord9(string9, 0, mixer2, 1);
AudioConnection          patchCord10(string10, 0, mixer2, 2);
AudioConnection          patchCord11(string11, 0, mixer2, 3);

AudioConnection          patchCordSubMix0(mixer0, 0, sumMix, 0);
AudioConnection          patchCordSubMix1(mixer1, 0, sumMix, 1);
AudioConnection          patchCordSubMix2(mixer2, 0, sumMix, 2);
AudioConnection          patchCordSubMix3(mixer3, 0, sumMix, 3);

AudioConnection          patchCordOutLeft(sumMix, 0, audioOut, 0);
AudioConnection          patchCordOutRight(sumMix, 0, audioOut, 1);


int strings=0;
  
//All commands are CC for simplicity sake
void stringsSetup()

{

const int finger_delay = 5;
const int hand_delay = 220;

  mixer0.gain(0, 0.25);
  mixer0.gain(1, 0.25);
  mixer0.gain(2, 0.25);
  mixer0.gain(3, 0.25);
  
  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);
  
  mixer2.gain(0, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  mixer2.gain(3, 0.25);
  
  sumMix.gain(0, .5);
  sumMix.gain(1, .5);
  sumMix.gain(2, .5);
}

  void stringPlayer(int pitch, uint16_t velocity)
  {
  Serial.print(pitch);
  Serial.print(" string# ");
  Serial.println(strings);
  Serial.print("Velocity ");
  Serial.print(velocity);
  Serial.println(" ");
  
  if (oldPitch==pitch)
  {
    strings = oldStrings;
  }
  else
  {
    ++strings;
  }
  
  switch (strings)
    {
    case 0: 
    string0.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 1:  
    string1.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 2: 
    string2.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 3: 
    string3.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 4: 
    string4.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 5: 
    string5.noteOn(MidiNotesHZ[pitch], velocity);
    break;  
    case 6: 
    string6.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 7: 
    string7.noteOn(MidiNotesHZ[pitch], velocity);
    break;  
    case 8: 
    string8.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 9: 
    string9.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 10: 
    string10.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    case 11: 
    string11.noteOn(MidiNotesHZ[pitch], velocity);
    break;
    
    default: 
    string0.noteOn(MidiNotesHZ[pitch], velocity);
    strings =0;
  }
  
  oldStrings = strings;
  oldPitch=pitch;
  
  }
