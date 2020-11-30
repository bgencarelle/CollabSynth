#define MAX_STRINGS 12
#define MIXER_COUNT 4 // 1 mixer for every 4 voices
#if MIXER_COUNT < 1
#error NEED MO STRINGS
#endif 

#define PITCHBEND_RANGE -(1.0/8192.0)  //should be 8192 >> or << by no more than say 5
#define DIV16BIT 1.0/65535.0

static volatile uint8_t stringVoice=0;
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
  
  for (uint32_t i = 0; i <MAX_STRINGS; i++)
  {
  stringArray[i].setDecayTime(decayFloat);
  }
  return;
}

inline void setPitchBend(uint32_t pitchBend)//pitchbend - currently is not optimal on bend down during decay
{
  double pitchDouble = (((double)pitchBend) * PITCHBEND_RANGE);
  double pitchResult = exp2f(pitchDouble);
    for (uint32_t i = 0; i <MAX_STRINGS; i++)
  {
  stringArray[i].setPitchBend(pitchResult);
  }
  return;
}
inline float setAttack (uint32_t velocity)
{
  return DIV16BIT *(float((velocity*velocity)<<2)+1020.0);
}

void sketchStringsInit()//initial setup
{
  float stringScaler =   (.5f*(double)MIXER_COUNT)/((double)MAX_STRINGS*1.0f);
  for (uint32_t i = 0; i < MIXER_COUNT; i++)
  {
    mixer[i].gain(0, stringScaler);
    mixer[i].gain(1, stringScaler);
    mixer[i].gain(2, stringScaler);
    mixer[i].gain(3, stringScaler);
    sumMix.gain(i,1);
  }
  stringArray[0].stringsSetup(1);

  setDecayVal(127);
  setPitchBend(0);
  return;
  
}

void stringPlayer(uint8_t midiNote=69, uint8_t velIn=127, uint8_t decayIn = 125)//two parameters from the midi section.  
{
  setDecayVal(decayIn);
  float velocity=  setAttack(velIn);

    if (++stringVoice > (MAX_STRINGS - 1))
    { 
     stringVoice = 0;
    } 
      stringArray[stringVoice].noteOff();
      stringArray[stringVoice].noteOn(midiNote, velocity);
//      Serial.print("NOTE: ");
//      Serial.println(midiNote);      
//      stringArray[stringVoice].noteOn(midiNote, velocity);
//      Serial.print("STRING VOICE: ");
//      Serial.println(stringVoice);      
return;
}
