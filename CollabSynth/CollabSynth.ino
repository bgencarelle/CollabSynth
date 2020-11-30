/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine

*/

#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>

#include "synth_karplusstrongMod.h"

#define MIDI_SERIAL Serial1


uint8_t noteTest = 30;


AudioOutputAnalogStereo  audioOut;
AudioMixer4              mainMixer4;

//
//AudioConnection          patchCordMainMixOut0(mainMixer4, 0, biquad1, 0);
//AudioConnection          patchCordMainMixOut1(mainMixer4, 0, biquad1, 1);

AudioConnection          patchCordMainMixOut0(mainMixer4, 0, audioOut, 0);
AudioConnection          patchCordMainMixOut1(mainMixer4, 0, audioOut, 1);
//AudioConnection          patchCordOut1(biquad1, 0, audioOut, 0);
//AudioConnection          patchCordOut2(biquad1, 0, audioOut, 1);



extern "C" char *sbrk(int32_t i);
int32_t FreeRam () {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}
MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);//creates the hardware MIDI

#include "GuitarMIDI.h"//this is the Karplus Strong stuff
#include "MidiConfig.h"//MIDI callbacks

void setup()
{
  for (uint32_t i = 0; i < 4; i++)
  {
    mainMixer4.gain(i, .25);
  }
  sketchStringsInit();//creates the strings
  AudioMemory(20);//allocate AudioMemory according to some magical thinking-
  Serial.begin(500000);//because lacking debugger
  midiSetup();//creates midi
  delayMicroseconds(70000);//settling time? this came with the example
  Serial.println("Begin");
}
int16_t pitch =0;
static uint32_t localFilterScoop = 0;

void loop()
{  
  if ( (abs)(pitch) >= 8191)
  {
    pitch=8191;
  }
  pitch = pitch -400;
  stringArray[0].filterScoop = 40;

  stringPlayer(noteTest, 120, 127);
  
  delayMicroseconds(20000);
  setPitchBend(pitch);
//
//  uint32_t availRam = FreeRam();
//  
//  Serial.println("Free RAM ");
//  Serial.println(availRam, DEC);
  
  delayMicroseconds(2000000);
  if (++noteTest > 105) noteTest = 0;




}
