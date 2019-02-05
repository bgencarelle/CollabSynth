/*   Initial synth engine for collaborative project.  
 *   3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine 
 *    
*/
#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>

AudioOutputAnalogStereo  audioOut;


#define MIDI_SERIAL Serial1

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);


float MidiNotesHZ[127];
#include "GuitarMIDI.h"
#include "MidiConfig.h"
void setup()
{
  AudioMemory(64);
  Serial.begin(9600);
  Serial.println("Begin");
  stringsSetup();
  midiSetup();
  float a = 440.0; // a is 440 hz...
  for (int x = 0; x < 127; ++x)
    {
       MidiNotesHZ[x] = (a / 32.0) * ( powf(2.0,(((float)x - 9.0) / 12.0)));
    }
}

void loop()
{
  midiB.read();
  

}
