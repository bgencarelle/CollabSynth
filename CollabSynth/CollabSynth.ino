/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine

*/
#include <MIDI.h>
#include <Audio.h>
 #include "synth_karplusstrongMod.h"
#include <Wire.h>
#define MIDI_SERIAL Serial1
#define A440 440.0

double MidiNotesDelay[128];//stores the size of the memory block for the strings
double MidiNotesFreq[128]; //frequency calculation for MIDI
uint16_t scalerMidiNote[128] = {0};//this scales the delay blocks to minimize memory
double midiNotesScaled[128]= {0}; //result

AudioOutputAnalogStereo  audioOut;

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);//creates the hardware MIDI

#include "GuitarMIDI.h"//this is the Karplus Strong stuff
#include "MidiConfig.h"//MIDI callbacks
void setup()
{
  double a = A440;//this is tuning standard
  for (int x = 0; x < 127; x++)
  {
    double xx = x;
    double freqCalc = a*exp2f((x-69.0)/12.0);
    MidiNotesDelay[x] = (AUDIO_SAMPLE_RATE_EXACT/freqCalc);
    MidiNotesFreq[x] = freqCalc;
  }
  AudioMemory(40);//allocate AudioMemory according to some magical thinking-
  Serial.begin(9600);//because lacking debugger
  Serial.println("Begin");
  stringsSetup();//creates the strings
  midiSetup();//creates midi
  delay(700);//settling time? this came with the example
}

void loop()
{
  midiB.read();//this is the only thing in our loop
  for (int x = 0; x < 127; x++)
  {
    delayMicroseconds(10000);
    
     stringPlayer(x, 127);
  }
   // Serial.println("Begin");
}
