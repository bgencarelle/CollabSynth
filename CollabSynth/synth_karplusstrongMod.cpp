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
//  #ifdef HARDWARE_RANDOM
  // This interrupt handler is called when a new random number is available; it reads the random number and clears the interrupt
  randomNum = TRNG->DATA.reg;                     // Read & store the TRNG data output
  if (randomCheck == randomNum)
  {
    delayMicroseconds(10);
    randGen();
  }
  else
  {
  TRNG->INTFLAG.bit.DATARDY = 1;             // Clear the DATA READY interrupt flag
  randomCheck = randomNum;
  }
//  #else
//
//  uint32_t lo = (uint32_t)randomNum;
//  uint32_t hi;
//  hi = multiply_16bx16t(16807, lo); // 16807 * (lo >> 16)
//  lo = 16807 * (lo & 0xFFFF);
//  lo += (hi & 0x7FFF) << 16;
//  lo += hi >> 15;
//  lo = (lo & 0x7FFFFFFF) + (lo >> 31);
//  randomNum=(int32_t)lo;
//  #endif

  
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
void AudioSynthKarplusStrongMod::setDecayTime(float decay)
{
  if (decay >= 1.0)
  {
   decayTime = 1.0;
  }
  else if (decay < 0.92)
  {
    decayTime = 0.915;
  }
  else
  {
    decayTime = decay;
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
    
    for (int i = 0; i < bufferLenPitch; i++) //initial load of tasty noise
    {
      randGen();
      buffer[i] = ((float)randomNum*floatScale*magnitude);

    }
    Serial.println(buffer[49]);
    
    
    uint32_t filterLoop = (octaveScaler>>3);
    if (filterLoop <= 2)
    {
      filterLoop = 2;
    }
    for (int j = 0; j<filterLoop; j++)
   {
    for (int i = 0; i < bufferLenPitch; i++)
    {
      if (i == 0)
      {
        buffer[0] = ((buffer[bufferLenPitch-1]) + (buffer[0]))*.500;
      }
      else
      {
        buffer[i] = ((buffer[i - 1]) + (buffer[i]))*.500 ;
      }
    }
    }
//         for (int i = 0; i < bufferLenPitch; i++)
//    {
//     buffer[i]=(buffer[i]);
//    }
     
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
    float out = ((in + prior) *.5*decayTime);
    *data++ = int16_t(out*32767.0);
    buffer[bufferIndex] = out;
    prior = in;
    if((increment++ >=octaveScaler)||octaveScaler==1)
    {      
      increment=0;
    if (++bufferIndex >= bufferLen) bufferIndex = 0;
    }
  }


  transmit(block);
  release(block);
}
