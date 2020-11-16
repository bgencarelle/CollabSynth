/*   Initial synth engine for collaborative project.
     3/Feb/2019 Initial commit to Github. Modified Audio libraries to increase sample rate and also fix issues with KP engine

*/
#define  isFeather 


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
char noteTest = 44;

AudioOutputAnalogStereo  audioOut;

MIDI_CREATE_INSTANCE(HardwareSerial, MIDI_SERIAL, midiB);//creates the hardware MIDI

#include "GuitarMIDI.h"//this is the Karplus Strong stuff
#include "MidiConfig.h"//MIDI callbacks

void setup()
{
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;    // Activate the CLK_TRNG_APB clock
  NVIC_SetPriority(TRNG_IRQn, 0);              // Set NVIC priority for TRNG to 0 
  NVIC_EnableIRQ(TRNG_IRQn);                   // Connect TRNG to NVIC 
  TRNG->INTENCLR.reg = TRNG_INTENCLR_DATARDY;  // Disable the TRNG interrupt
  TRNG->CTRLA.reg = TRNG_CTRLA_ENABLE;         // Enable the TRNG peripheral  
  
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
  //midiB.read();//this is the only thing in our loop

       if (noteTest >=128) 
       {
        noteTest =44;
       }
    delayMicroseconds(1000000);
    
     stringPlayer(noteTest++, 127);

     stringPlayer(noteTest+3, 127);
     stringPlayer(noteTest+6, 127);
     stringPlayer(noteTest+9, 127);
  

    Serial.println(noteTest,DEC);
}
