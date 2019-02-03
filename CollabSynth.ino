/*   Tascam Serial Control for Midistudios and devices that use the Accessory 2 port.

//ff time 1:23 
//rw time 1:16 
//pin 13 on the controller handles chaseMode 
//pin 5 is direction-rw is low ff/play/stop is high 
//pin 6 is tach pulse  3.22 hz is play, rw is between 25 and ~78hz ends low, ff is 44 to 30 hz,play is 2.92hz 

   The goal of this project is to create an open device that can replace the obsolete and
   all but impossible-to-find sync devices for various Tascam tape machines.
  See the Readme.md for more details
   2018 Ben Gencarelle
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
  AudioMemory(128);
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
