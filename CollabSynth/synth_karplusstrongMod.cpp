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
#include "synth_karplusstrongMod.h"

#ifdef FLOATSCALE16
float AudioSynthKarplusStrongMod::floatScale = FLOATSCALE16;
#else
float AudioSynthKarplusStrongMod::floatScale = FLOATSCALE32;
#endif

inline void AudioSynthKarplusStrongMod::randGen()                            // Called when TRNG data is available
{
  // This interrupt handler is called when a new random number is available; it reads the random number and clears the interrupt

  randomNum = TRNG->DATA.reg;                     // Read & store the TRNG data output
  if (randomCheck == randomNum)
  {
    randGen();
  }
  TRNG->INTFLAG.bit.DATARDY = 1;             // Clear the DATA READY interrupt flag
  randomCheck = randomNum;
  randomFloat = (float)(randomNum)*magnitude;

}
void AudioSynthKarplusStrongMod::setPitchBend(float pitch)
{

  if (pitch < 0.125 )
  {
    globalPitch = .125;
  }
  else if (pitch >= 8.0)
  {
    globalPitch = 8.0;
  }
  else
  {
    globalPitch = pitch;
  }
}
void AudioSynthKarplusStrongMod::setDecayTime(int32_t decay)
{
  if (decay >= 32767)
  {
   decayTime = 1.0;
  }
  else if (decay < 30000)
  {
    decayTime = 30000.0 / 32767.0;
  }
  else
  {
    decayTime = (float)decay / 32767.0;
  }
}
void AudioSynthKarplusStrongMod::checkLength(void)
{
  uint32_t testSize = uint32_t(bufferLen * globalPitch);
  if (testSize <= 5)
  {
    bufferLenPitch = 5;
  }
  else if (testSize >= MAX_BUFF - 1) {
    bufferLenPitch = MAX_BUFF - 1;
  }
  else
  {
    bufferLenPitch = testSize;
  }
}
void AudioSynthKarplusStrongMod::update(void)
{
  audio_block_t *block;
  if (state == 0)
    return;
  if (state == 1)
  {
    for (int i = 0; i < bufferLenPitch - 1; i++) //initial load of tasty noise
    {
      randGen();
      buffer[i] = (randomFloat);
    }
    uint32_t filterLoop = (AVG_LOOPS) - 1;
    if (filterLoop <= 2)
    {
      filterLoop = 2;
    }
    for (int j = 0; j<filterLoop; j++)
    {
    for (int i = 0; i < bufferLenPitch - 1; i++)
    {
      if (i == 0)
      {
        buffer[0] = ((buffer[bufferLenPitch-1]) + (buffer[0])) * .5;
      }
      else
      {
        buffer[i] = ((buffer[i - 1]) + (buffer[i])) * .5;
      }
    }
    }
    state = 2;
  }
  block = allocate();
  if (!block)
  {
    state = 0;
    return;
  }

  float prior;

  if (bufferIndex > 0) {
    prior = buffer[bufferIndex - 1];
  } else {
    prior = buffer[bufferLenPitch - 1];
  }
  int16_t *data = block->data;

  checkLength();

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float in = buffer[bufferIndex];
    float avg = (in + prior) * .5;
    buffer[bufferIndex] = avg;
    // uint32_t compare = (i+1) % (octaveScaler);

    int16_t out = int16_t(avg);
    *data++ = int16_t(out);

    prior = in;

    {
      if (++bufferIndex >= bufferLenPitch - 1)
      {
        bufferIndex = 0;
      }
    }
  }

  transmit(block);
  release(block);
}
