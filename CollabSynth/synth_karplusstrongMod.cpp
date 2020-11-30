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

    AudioSynthKarplusStrongMod::setupFlag = 1;
    double a = A440;//this is tuning standard

    octaveDivSetup();
    for (uint32_t x = 0; x < 127; x++)
    {
      double freqCalc = a * exp2((( ((double)(x)) - 69.0f) / 12.0f));
      double oneOverOct = (1.0 / (double)(octaveDivMidiNote[x]));
      midiNotesDelay[x] = .75f + ((((AUDIO_SAMPLE_RATE_EXACT / freqCalc)) * (oneOverOct))) ;
      midiNotesFreq[x] = freqCalc;
    }
    firstStart = 0;
    
  }
return;
}

inline void AudioSynthKarplusStrongMod::randGen()                            // Called when TRNG data is available
{
  randomNum = TRNG->DATA.reg;                     // Read & store the TRNG data output
  
  if (randomCheck == randomNum)
  {
    randGen();
  }
  else
  {
    TRNG->INTFLAG.bit.DATARDY = 1;             // Clear the DATA READY interrupt flag
    randomCheck = randomNum;
    return;
  }
}

void AudioSynthKarplusStrongMod::octaveDivSetup()//might be a better way to do this, but right now it is useful to be able to do it this way
{ // each octave gets a different scaler. This scaler is also used in the actual string synth
  for (uint32_t i = 0; i <= 11; i++)
  {
    octaveDivMidiNote[i] = 1 << 4 ;
  }
  for (uint32_t i = 12; i <= 23; i++)
  {
    octaveDivMidiNote[i] = 1 << 3;
  }
  for (uint32_t i = 24; i <= 35; i++)
  {
    octaveDivMidiNote[i] = 1 << 2;
  }
  for (uint32_t i = 36; i <= 47; i++)
  {
    octaveDivMidiNote[i] = 1 << 1;
  }
  for (uint32_t i = 48; i <= 59; i++)
  {
    octaveDivMidiNote[i] = 1 ;
  }
  for (uint32_t i = 60; i <= 71; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (uint32_t i = 72; i <= 83; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (uint32_t i = 84; i <= 95; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (uint32_t i = 96; i <= 107; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  for (uint32_t i = 108; i <= 127; i++)
  {
    octaveDivMidiNote[i] = 1;
  }
  return;
}
void AudioSynthKarplusStrongMod::setPitchBend(float pitch)
{
  if (pitch < 0.250f )
  {
    globalPitch = .250f;
  }
  else if (pitch >= 2.0f)
  {
    globalPitch = 2.0f;
  }
  else
  {
    globalPitch = pitch;
  }
  
  return;
}
void AudioSynthKarplusStrongMod::setDecayTime(float decay)
{
  if (decay >= 1.0f)
  {
    decayTime = .49999f;
  }
  else if (decay < 0.92f)
  {
    decayTime = 0.45075f;
  }
  else
  {
    decayTime = decay * .49999f;
  }
  return;
}
void AudioSynthKarplusStrongMod::checkLength(void)
{
  Serial.println(midiNotesDelay[noteIn]);
  uint32_t testSize = uint32_t(midiNotesDelay[noteIn]);
  Serial.println(testSize);
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
  return;
}

void AudioSynthKarplusStrongMod::loadBuffer()
    {
    for (uint32_t i = 0; i < MAX_BUFF; i++) //initial load of tasty noise
    {
      randGen();
      buffer[i] = ((float)randomNum * magnitude* floatScale);

    }
    uint32_t filterLoop = AudioSynthKarplusStrongMod::filterScoop;
    
    if (filterLoop <= AVG_LOOPS)
    {
      filterLoop = AVG_LOOPS;
    }
    for (uint32_t j = 0; j < 4; j++)
    {
      
      for (uint32_t i = 0; i < MAX_BUFF; i++)
      {
        if (i == 0)
        {
          buffer[0] = ((buffer[MAX_BUFF - 1])* .500 + (buffer[0])* .500);
        }
        else
        {
          buffer[i] = ((buffer[i - 1])* .500 + (buffer[i])* .500)  ;
        }
  
      }
     
      
    }
    for (uint32_t i = 0; i < MAX_BUFF; i++)
    {
      
      buffer[i] = (buffer[i] * (1.0 + ((float)filterLoop / 256.0)));
      
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
    octaveScaler = octaveDivMidiNote[noteIn];
    loadBuffer();
    state = 2;
    return;
  }
  if (state == 2)
  {
  
  //checkLength();
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
  float dataFloatConv;
  for (uint32_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
  {
    float in = buffer[bufferIndex];
    float out = ((in + prior) * decayTime);
    dataFloatConv = out ;
    *data++ = int16_t(dataFloatConv);
    buffer[bufferIndex] = out;
    prior = in;
    if ((increment++ >= (octaveScaler - 1)) || octaveScaler <= 1)
    {
      increment = 0;
      if (++bufferIndex >= bufferLenPitch) bufferIndex = 0;
    }
  }

  transmit(block);
  release(block);
  }
}
volatile float AudioSynthKarplusStrongMod::globalPitch=1.0f;
uint32_t AudioSynthKarplusStrongMod::octaveDivMidiNote[128];//this scales the delay blocks to minimize
uint32_t AudioSynthKarplusStrongMod::filterScoop=AVG_LOOPS;//this scales the delay blocks to minimize
uint32_t AudioSynthKarplusStrongMod::midiNotesDelay[128];//stores the size of the memory block for the strings
double AudioSynthKarplusStrongMod::midiNotesFreq[128]; //frequency calculation for MIDI
boolean AudioSynthKarplusStrongMod::setupFlag = 1;
