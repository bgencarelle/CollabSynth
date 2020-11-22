/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine
 
*/
#define SAMPLERATEOVERRIDE 1

#include <MIDI.h>
#include <Audio.h>

#include "synth_karplusstrongMod.h"


#include <Wire.h>
#define MIDI_SERIAL Serial1
#define A440 440.00
#define DIV16BIT 1.0/65536.0

int32_t MidiNotesDelay[128];//stores the size of the memory block for the strings
double MidiNotesFreq[128]; //frequency calculation for MIDI
uint8_t octaveDivMidiNote[128] = {0};//this scales the delay blocks to minimize memory
uint32_t midiNotesScaled[128]= {0}; //result
char noteTest = 60;

AudioOutputAnalogStereo  audioOut;
AudioMixer4              mainMixer4;
AudioConnection          patchCordMainMix0(mainMixer4, 0, audioOut, 0);
AudioConnection          patchCordMainMix1(mainMixer4, 0, audioOut, 1);

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);//creates the hardware MIDI

#include "GuitarMIDI.h"//this is the Karplus Strong stuff
#include "MidiConfig.h"//MIDI callbacks

void setup()
{
    

  for (int i = 0; i < 4; i++)
  {
    mainMixer4.gain(i, .25);
  }
  double a = A440;//this is tuning standard
  for (int x = 0; x < 127; x++)
  {
    double xx = (((double)x-69.0f)*0.0833333333f);
    double freqCalc = a*exp2(xx);
    MidiNotesDelay[x] = uint32_t(((double)AUDIO_SAMPLE_RATE_EXACT/freqCalc)+.5f);
    MidiNotesFreq[x] = freqCalc;
  }
  AudioMemory(40);//allocate AudioMemory according to some magical thinking-
  Serial.begin(9600);//because lacking debugger
 
  
  stringsSetup();//creates the strings
  midiSetup();//creates midi
  delayMicroseconds(700000);//settling time? this came with the example
   Serial.println("Begin");
}
void loop()
{
  //midiB.read();//this is the only thing in our loop
     stringPlayer(69,127,127); 

     Serial.print("NOTE Frequency: ");
       Serial.println(MidiNotesFreq[69]);
     Serial.print("delayLength ");
       Serial.println(MidiNotesDelay[69]*MidiNotesFreq[69]);
      
    delayMicroseconds(10000000);


    
}
