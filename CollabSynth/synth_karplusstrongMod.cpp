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
void AudioSynthKarplusStrongMod::stringsSetup(boolean firstStart)
{
  
  if (firstStart)
  {
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_TRNG;    // Activate the CLK_TRNG_APB clock
    NVIC_SetPriority(TRNG_IRQn, 0);              // Set NVIC priority for TRNG to 0
    NVIC_EnableIRQ(TRNG_IRQn);                   // Connect TRNG to NVIC
    TRNG->INTENCLR.reg = TRNG_INTENCLR_DATARDY;  // Disable the TRNG interrupt
    TRNG->CTRLA.reg = TRNG_CTRLA_ENABLE;         // Enable the TRNG peripheral
    
    firstStart=0;
    double a = A440;//this is tuning standard
    
    octaveDivSetup();
    for (int x = 1; x < 127; x++)
    {
      double freqCalc = a * exp2((((double)x - 69.0f) / 12.0f));
      MidiNotesDelay[x] = ((AUDIO_SAMPLE_RATE_EXACT / freqCalc) + .5f) / octaveDivMidiNote[x];
      MidiNotesFreq[x] = freqCalc;
    }
    delayMicroseconds(10);
  }

} 

inline void AudioSynthKarplusStrongMod::randGen()                            // Called when TRNG data is available
{
  //  #ifdef HARDWARE_RANDOM
  // This interrupt handler is called when a new random number is available; it reads the random number and clears the interrupt
  randomNum = TRNG->DATA.reg;                     // Read & store the TRNG data output
  
  if (randomCheck == randomNum)
  {
    delayMicroseconds(1);
    randGen();
  }
  else
  {
    TRNG->INTFLAG.bit.DATARDY = 1;             // Clear the DATA READY interrupt flag
    randomCheck = randomNum;
    return;
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


void AudioSynthKarplusStrongMod::octaveDivSetup()//might be a better way to do this, but right now it is useful to be able to do it this way
{ // each octave gets a different scaler. This scaler is also used in the actual string synth
  for (int i = 1; i <= 11; i++)
  {
    octaveDivMidiNote[i] = 1 << 5;
  }
  for (int i = 12; i <= 23; i++)
  {
    octaveDivMidiNote[i] = 1 << 4;
  }
  for (int i = 24; i <= 35; i++)
  {
    octaveDivMidiNote[i] = 1 << 3;
  }
  for (int i = 36; i <= 47; i++)
  {
    octaveDivMidiNote[i] = 1 << 2;
  }
  for (int i = 48; i <= 59; i++)
  {
    octaveDivMidiNote[i] = 1 << 1;
  }
  for (int i = 60; i <= 71; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (int i = 72; i <= 83; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (int i = 84; i <= 95; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (int i = 96; i <= 107; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (int i = 108; i <= 127; i++)
  {
    octaveDivMidiNote[i] = 1;
  }

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
    decayTime = .49999;
  }
  else if (decay < 0.92)
  {
    decayTime = 0.45075;
  }
  else
  {
    decayTime = decay * .49999;
  }
}
void AudioSynthKarplusStrongMod::checkLength(void)
{
  uint32_t testSize = uint32_t(MidiNotesDelay[noteIn] * globalPitch);
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
    checkLength();
    octaveScaler=octaveDivMidiNote[noteIn];
    for (int i = 0; i < bufferLenPitch; i++) //initial load of tasty noise
    {
      randGen();
      buffer[i] = ((float)randomNum * magnitude);
    }
    
  Serial.print("here's your fart value, bitch");
  Serial.println(bufferLenPitch);
    
    uint32_t filterLoop = AVG_LOOPS * (octaveScaler >> 3);
    if (filterLoop <= 1)
    {
      filterLoop = 1;
    }
    for (int j = 0; j < filterLoop; j++)
    {
      for (int i = 0; i < bufferLenPitch; i++)
      {
        if (i == 0)
        {
          buffer[0] = ((buffer[bufferLenPitch - 1]) + (buffer[0])) * .500;
        }
        else
        {
          buffer[i] = ((buffer[i - 1]) + (buffer[i])) * .500 ;
        }
      }
    }

    for (int i = 0; i < bufferLenPitch; i++)
    {
      buffer[i] = (buffer[i] * magnitude * floatScale);
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

  volatile uint32_t increment;

  for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
    float in = buffer[bufferIndex];
    float out = ((in + prior) * decayTime);
    *data++ = int16_t(out);
    buffer[bufferIndex] = out;
    prior = in;

    if ((increment++ >= octaveScaler - 1) || octaveScaler <= 1)
    {
      increment = 0;
      if (++bufferIndex >= bufferLenPitch) bufferIndex = 0;
    }
  }


  transmit(block);
  release(block);
}   
    uint8_t AudioSynthKarplusStrongMod::octaveDivMidiNote[127];//this scales the delay blocks to minimize 
    uint32_t AudioSynthKarplusStrongMod::MidiNotesDelay[127];//stores the size of the memory block for the strings
    double AudioSynthKarplusStrongMod::MidiNotesFreq[127]; //frequency calculation for MIDI
    uint32_t AudioSynthKarplusStrongMod::midiNotesScaled[127]; //result
