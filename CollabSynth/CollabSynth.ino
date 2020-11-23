/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine
 
*/
#define SAMPLERATEOVERRIDE 1

#include <MIDI.h>
#include <Audio.h>

#include "synth_karplusstrongMod.h"


#include <Wire.h>
#define MIDI_SERIAL Serial1

#define DIV16BIT 1.0/65535.0

char noteTest = 1;

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
  
  stringsSetup();//creates the strings

  AudioMemory(70);//allocate AudioMemory according to some magical thinking-
  Serial.begin(9600);//because lacking debugger
 
  
  midiSetup();//creates midi
  delayMicroseconds(700000);//settling time? this came with the example
   Serial.println("Begin");
}
void loop()
{
  //midiB.read();//this is the only thing in our loop

     stringPlayer(noteTest,127,127);
    if(++noteTest>=110) (noteTest=1);
        delayMicroseconds(500000);


    
}
