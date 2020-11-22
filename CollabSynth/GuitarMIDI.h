#define MAX_STRINGS 12
#define MIXER_COUNT MAX_STRINGS/4 // 1 mixer for every 4 voices
#define PITCHBEND_RANGE -(1.0/8192.0)  //should be 8192 >> or << by no more than say 5

volatile uint8_t stringVoice=0;
volatile uint8_t lastNote[MAX_STRINGS];

// Special thanks to Matthew Rahtz - http://amid.fish/karplus-strong/

AudioSynthKarplusStrongMod  stringArray[MAX_STRINGS];

AudioMixer4              mixer[MIXER_COUNT];
AudioMixer4              sumMix;

AudioConnection          patchCord0(stringArray[3], 0, mixer[0], 0);
AudioConnection          patchCord1(stringArray[2], 0, mixer[0], 1);
AudioConnection          patchCord2(stringArray[1], 0, mixer[0], 2);
AudioConnection          patchCord3(stringArray[0], 0, mixer[0], 3);
AudioConnection          patchCordSubMix0(mixer[0], 0, sumMix, 0);
#if (MIXER_COUNT > 1)
AudioConnection          patchCord4(stringArray[4], 0, mixer[1], 0);
AudioConnection          patchCord5(stringArray[5], 0, mixer[1], 1);
AudioConnection          patchCord6(stringArray[6], 0, mixer[1], 2);
AudioConnection          patchCord7(stringArray[7], 0, mixer[1], 3);
AudioConnection          patchCordSubMix1(mixer[1], 0, sumMix, 1);
#endif
#if (MIXER_COUNT > 2)
AudioConnection          patchCord8(stringArray[8], 0, mixer[2], 0);
AudioConnection          patchCord9(stringArray[9], 0, mixer[2], 1);
AudioConnection          patchCord10(stringArray[10], 0, mixer[2], 2);
AudioConnection          patchCord11(stringArray[11], 0, mixer[2], 3);
AudioConnection          patchCordSubMix2(mixer[2], 0, sumMix, 2);
#endif

AudioConnection          patchCordOutLeft(sumMix, 0, mainMixer4, 0);
AudioConnection          patchCordOutRight(sumMix, 0, mainMixer4, 1);



void octaveDivSetup()//might be a better way to do this, but right now it is useful to be able to do it this way
{// each octave gets a different scaler. This scaler is also used in the actual string synth
  for (int i = 0; i <= 11; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  } 
  for (int i = 12; i <= 23; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  } 
  for (int i = 24; i <= 35; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
  for (int i = 36; i <= 47; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
  for (int i = 48; i <= 59; i++)
  {
    octaveDivMidiNote[i] = 1<<1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
  for (int i = 60; i <= 71; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
    for (int i = 72; i <= 83; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
    for (int i = 84; i <= 95; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
    for (int i = 96; i <= 107; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
    for (int i = 108; i <= 127; i++)
  {
    octaveDivMidiNote[i] = 1;
    midiNotesScaled[i] = MidiNotesDelay[i]/octaveDivMidiNote[i];
  }
  
}

inline void setDecayVal(uint32_t modValue)// this sets the decay time of the strings. It is currently mapped to the mod wheel, but is technically actually a sustain
{
  float value = 28703+ float((modValue) << 5) ; //need an offset, otherwise too severe
  float decayFloat;
  
    if (value >= 32767)
  {
   decayFloat = 1.0;
  }
  else if (value < 30000)
  {
    decayFloat = 0.915;
  }
  else
  {
    decayFloat = value / 32767.0;
  }
  
  for (int i = 0; i <MAX_STRINGS; i++)
  {
  stringArray[i].setDecayTime(decayFloat);
  }
  return;
}

inline void setPitchBend(uint32_t pitchBend)//pitchbend - currently is not optimal on bend down during decay
{
  double pitchDouble = ((double)pitchBend) * PITCHBEND_RANGE;
  double pitchResult = exp2f(pitchDouble);
    for (int i = 0; i <MAX_STRINGS; i++)
  {
  stringArray[i].setPitchBend(pitchResult);
  }
  return;
}
inline float setAttack (uint32_t velocity)
{
  
  return DIV16BIT *(float((velocity*velocity)<<2)+1020.0);
}
void stringsSetup()//initial setup
{
  octaveDivSetup();
  float stringScaler =  4.0/(float)MAX_STRINGS;

  for (int i = 0; i < MIXER_COUNT; i++)
  {
    mixer[i].gain(0, stringScaler);
    mixer[i].gain(1, stringScaler);
    mixer[i].gain(2, stringScaler);
    mixer[i].gain(3, stringScaler);
    
    sumMix.gain(i,1);
  }
  stringArray[0].randGenStartup(1);//

  setDecayVal(127);
  setPitchBend(0);
  return;
  
}

void stringPlayer(uint8_t midiNote=60, uint8_t velIn=127, uint8_t decayIn = 125)//two parameters from the midi section.  
{
  setDecayVal(decayIn);
  float velocity=  setAttack(velIn);

    if (++stringVoice > MAX_STRINGS - 1)
    { 
      
     stringArray[MAX_STRINGS - 1].noteOff();
     stringVoice = 0;
    } 
    else
    {
      stringArray[stringVoice-1].noteOff();
    }
      stringArray[stringVoice].noteOn(MidiNotesDelay[midiNote], velocity, octaveDivMidiNote[midiNote]);

      
      Serial.print("STRING VOICE: ");
      Serial.println(stringVoice);
      
return;

}
