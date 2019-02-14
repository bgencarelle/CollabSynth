/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine

*/
#include <MIDI.h>
#include <Audio.h>
#include <Wire.h>
#define MIDI_SERIAL Serial1

double MidiNotesDelay[128];
double MidiNotesFreq[128];
uint16_t scalerMidiNote[128] = {0};
double midiNotesScaled[128]= {0};
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
  double a = 440.0;
  for (int x = 0; x < 127; x++)
  {
    double xx = x;
    double freqCalc = a*exp2f((x-69.0)/12.0);
    MidiNotesDelay[x] = (AUDIO_SAMPLE_RATE_EXACT/freqCalc);
    MidiNotesFreq[x] = freqCalc;
  }
  AudioMemory(40);
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
