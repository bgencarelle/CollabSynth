/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine
 
*/

#include <MIDI.h>
#include <Audio.h>

#include "synth_karplusstrongMod.h"


#include <Wire.h>
#define MIDI_SERIAL Serial1

#define DIV16BIT 1.0/65535.0

uint8_t noteTest=30;

//Audio Effect BitCrusher defs here
AudioEffectBitcrusher   left_BitCrusher;
AudioEffectBitcrusher   right_BitCrusher;

AudioOutputAnalogStereo  audioOut;
AudioMixer4              mainMixer4;
AudioConnection          patchCordMainMix0(mainMixer4, 0, left_BitCrusher, 0);
AudioConnection          patchCordMainMix1(mainMixer4, 0, right_BitCrusher, 1);
AudioConnection          patchCordMainMix2(left_BitCrusher, 0, audioOut, 0);
AudioConnection          patchCordMainMix3(right_BitCrusher, 0, audioOut, 1);


//BitCrusher
int current_CrushBits = 16; //this defaults to passthrough.
int current_SampleRate = 44100; // this defaults to passthrough.


MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);//creates the hardware MIDI

#include "GuitarMIDI.h"//this is the Karplus Strong stuff
#include "MidiConfig.h"//MIDI callbacks

void setup()
{
   
  
  // Bitcrusher
  left_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  left_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
  //Bitcrusher

  for (int i = 0; i < 4; i++)
  {
    mainMixer4.gain(i, .25);
  }
  
  sketchStringsInit();//creates the strings

  AudioMemory(60);//allocate AudioMemory according to some magical thinking-
  Serial.begin(200000);//because lacking debugger
 
  
  midiSetup();//creates midi
  delayMicroseconds(700000);//settling time? this came with the example
   Serial.println("Begin");
}

void loop()
{
  static uint32_t localFilterScoop=1;

  //midiB.read();//this is the only thing in our loop
  current_SampleRate=4000;
  Serial.print("local sample Rateish ");
  Serial.println(current_SampleRate,DEC);
  left_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  left_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.bits(current_CrushBits); //set the crusher to defaults. This will passthrough clean at 16,44100
  right_BitCrusher.sampleRate(current_SampleRate); //set the crusher to defaults. This will passthrough clean at 16,44100
     stringArray[0].filterScoop = localFilterScoop;
    Serial.println("noteTest: ");
    Serial.println(noteTest,DEC);
    stringPlayer(39,120,120);

    delayMicroseconds(30000000);
     if (++noteTest >120) noteTest=0;

  
    Serial.println("stringArray[1].filterScoop");
    Serial.println(stringArray[1].filterScoop,DEC);
    for (int i=0; i<=7; i++)
    {
      Serial.print("I ");
      Serial.println(i);
    }
  //    
    
   // if(++noteTest>=110) (noteTest=);



    
}
