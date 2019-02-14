/*   Initial synth engine for collaborative project.  
 *   3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine 
 *    
*/
#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>
#define MIDI_SERIAL Serial1

double MidiNotesHZ[127];
AudioOutputAnalogStereo  audioOut;

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);
  uint32_t i = 0;
  uint32_t x = 0;
  uint32_t lo = 1;
  int64_t randSource = 1;

#include "GuitarMIDI.h"
#include "MidiConfig.h"
void setup()
{
  double a=440.0;
  for (int x = 0; x<127; x++)
  {
    MidiNotesHZ[x] = (a/32.0) * ( powf(2.0,(((float)x-9.0)/12.0)));
  }
  AudioMemory(30);
  Serial.begin(9600);
  Serial.println("Begin");
  
  stringsSetup();
  midiSetup();
    delay(700);
}

void loop()
{
 midiB.read();
}
