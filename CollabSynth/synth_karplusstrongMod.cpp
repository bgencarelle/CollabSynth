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

volatile uint32_t randomNum = 1;
volatile uint32_t randomCheck = 1;
boolean startup = 1;

void trngSetup(void) //setup RNG for SAMD51
{

}
// This interrupt handler is called when a new random number is available; it reads the random number and clears the interrupt
void randGen()                            // Called when TRNG data is available
{
#if defined(__SAMD51__)
  if (startup == 1)
  {
 
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;    // Activate the CLK_TRNG_APB clock
  NVIC_SetPriority(TRNG_IRQn, 0);              // Set NVIC priority for TRNG to 0
  NVIC_EnableIRQ(TRNG_IRQn);                   // Connect TRNG to NVIC
  TRNG->INTENCLR.reg = TRNG_INTENCLR_DATARDY;  // Disable the TRNG interrupt
  TRNG->CTRLA.reg = TRNG_CTRLA_ENABLE;         // Enable the TRNG peripheral
    startup = 0;
    delayMicroseconds(10);
  }
  
  randomNum = TRNG->DATA.reg;                     // Read & store the TRNG data output
  if (randomCheck == randomNum)
    {
      delayMicroseconds(1);
      randGen();
    }
  TRNG->INTFLAG.bit.DATARDY = 1;             // Clear the DATA READY interrupt flag
  randomCheck=randomNum;
#else
  uint32_t hi;
  hi = multiply_16bx16t(16807, randomNum); // 16807 * (randomNum >> 16)
  randomNum = 16807 * (randomNum & 0xFFFF); 
  randomNum += (hi & 0x7FFF) << 16; 
  randomNum += hi >> 15; 
  randomNum = (randomNum & 0x7FFFFFFF) + (randomNum >> 31); 

 
#endif

}
 int32_t AudioSynthKarplusStrongMod::decayTime;
 float AudioSynthKarplusStrongMod::globalPitch;
 int32_t   AudioSynthKarplusStrongMod::magnitude;

 void AudioSynthKarplusStrongMod::setPitchBend(float pitch)
 {

   if (pitch <0.125 )
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
 void AudioSynthKarplusStrongMod::setDecayTime(uint16_t decay)
 {
   if (decay >=32767)
   {
     AudioSynthKarplusStrongMod::decayTime = 32767;
   }
   else if (decay < 30000)
   {
     AudioSynthKarplusStrongMod::decayTime = 30000;
   }
   else
   {
       AudioSynthKarplusStrongMod::decayTime = decay;
   }
 }
 void AudioSynthKarplusStrongMod::checkLength(void)
 {
   uint16_t testSize = bufferLen * globalPitch;
   if (testSize <= 5)
   {
     bufferLenPitch = 5;
   }
   else if (testSize>=MAX_BUFF-1) {
     bufferLenPitch = MAX_BUFF-1;
   }
   else
   {
     bufferLenPitch = testSize;
   }
 }
 void AudioSynthKarplusStrongMod::update(void)
 {
  audio_block_t *block;

  if (state == 0) return;

  if (state == 1) {
    randGen();
    for (int i=0; i < MAX_BUFF; i++) {
      randGen();
      buffer[i] = signed_multiply_32x16b(magnitude, randomNum);
    }
   // seed = randomNum;
    state = 2;
  }
  if (state == 2)
  {
  block = allocate();
  if (!block) {
    state = 0;
    return;
  }

  int16_t prior;
  if (bufferIndex > 0) {
    prior = buffer[bufferIndex - 1];
  } else {
    prior = buffer[bufferLenPitch - 1];
  }
  int16_t *data = block->data;

 checkLength();
  for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {

        int16_t in = buffer[bufferIndex];
        int32_t  avg = (in +prior);
        int16_t out = signed_multiply_32x16b(avg, decayTime);
    int16_t compare = i%(globalScaler<<1);
    prior = in;

    buffer[bufferIndex] = out;
   // if (compare ==0)
    {
        if (++bufferIndex >= bufferLenPitch) bufferIndex = 0;
    }

  *data++ = out;
  }

  transmit(block);
  release(block);
 }
 }
