/* Audio Library for Teensy 3.X
 * Copyright (c) 2016, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef synth_karplusstrongMod_h_
#define synth_karplusstrongMod_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "utility/dspinst.h"
#define MAX_BUFF 1000
#define AVG_LOOPS 32




class AudioSynthKarplusStrongMod : public AudioStream
{
public:

  static  int32_t decayTime;
  static double globalPitch;
  uint32_t bufferLen;
  uint32_t bufferLenPitch;
  int32_t static  magnitude; // current output
  AudioSynthKarplusStrongMod() : AudioStream(0, NULL) {
    state = 0;
  }
  void noteOn(uint32_t len=440, int32_t velocity = 65535, uint32_t octaveDiv = 1)
  {
    setDecayTime(decayTime);
    octaveScaler = octaveDiv;
        if (len > MAX_BUFF-1) len = MAX_BUFF-1;//for pitch shifting
    bufferLen = len;
    bufferIndex = 0;
    if (velocity >=512)
    {
      magnitude = 65536-(int32_t)velocity;

    } else 
    {
      noteOff();
      return;
    } 

    state = 1;
    return;
  }

  void noteOff() {
    int32_t oldDecay = decayTime;
    setDecayTime(1);
    delayMicroseconds(10);
    setDecayTime(oldDecay);
    state = 0;
    return;
  }
  static void setPitchBend(double pitch = 1.0);
  static void setDecayTime(int32_t decay = 32767);
  void inline checkLength(void);
  virtual void update(void);
private:
  uint8_t  state;     // 0=steady output, 1=begin on next update, 2=playing
  uint32_t bufferIndex;

  static uint32_t seed;  // must start at 1
  volatile int32_t buffer[MAX_BUFF]; // TODO: dynamically use audio memory blocks
  int32_t octaveScaler;
};

#endif
