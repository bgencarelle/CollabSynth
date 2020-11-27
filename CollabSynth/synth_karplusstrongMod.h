/* Audio Library for Teensy 3.X
   Copyright (c) 2016, Paul Stoffregen, paul@pjrc.com

   Development of this audio library was funded by PJRC.COM, LLC by sales of
   Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
   open source software by purchasing Teensy or other PJRC products.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice, development funding notice, and this permission
   notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#ifndef synth_karplusstrongMod_h_
#define synth_karplusstrongMod_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"
#define MAX_BUFF 800
#define AVG_LOOPS 4
#define FLOATSCALE32  32767.0/2147483648.0
//#define FLOATSCALE16 32767.0/32768.0
#define A440 440.00

class AudioSynthKarplusStrongMod : public AudioStream
{
  public:
  
    AudioSynthKarplusStrongMod() : AudioStream(0, NULL) {
      state = 0;
    }
#ifdef FLOATSCALE16
    int16_t randomNum=1;
    int16_t randomCheck=1;
#else
    int32_t randomNum=1;
    int32_t randomCheck=1;
#endif
    float randomFloat;
    static float floatScale;
    float decayTime;
    float globalPitch;
    uint32_t noteIn;
    uint32_t bufferLenPitch;
    float magnitude; // current outpute 
    static double midiNotesFreq[128]; //frequency calculation for MIDI
    static uint32_t midiNotesDelay[128]; //result
    static void stringsSetup(boolean firstStart=0);
    static uint32_t octaveDivMidiNote[128];//this scales the delay blocks to minimize memory
    static void octaveDivSetup();
    static uint32_t filterScoop;
    void noteOff() 
    {
        for (int i = 0; i < MAX_BUFF; i++)
      { buffer[i] = 0.0;
      }
      state = 0;
      return;
    }
    
    void noteOn(uint32_t note = 69, float vel=1.0)
    {
      if (note > 120) note=120; //for pitch shifting
      if (note <1) note = 1;
      noteIn = note;
      bufferIndex = 0;
      if (vel >1.0)
       {
        magnitude = 1.0;
       }
      else if (vel > .014)
      {
        magnitude = vel;
      } else
      {
        magnitude=0.0;
        noteOff();
      }
      state = 1;
      return;
    }
    void randGen();
    void setPitchBend(float pitch = 1.0);
    void setDecayTime(float decay = 1.0);
    void inline checkLength(void);
    virtual void update(void);
    
  private:
    uint8_t  state;     // 0=steady output, 1=begin on next update, 2=playing
    uint32_t bufferIndex;
    float buffer[MAX_BUFF]; // TODO: dynamically use audio memory blocks
    int32_t octaveScaler;
    static uint32_t seed;
    static boolean setupFlag;
};

#endif
