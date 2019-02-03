

#define CC_CHANNEL 16//everything on one CC makes easier
//transport control
#define AllStop 0x00
#define RecStrobe 0x01
#define PlayOn 0x02
#define FFRew 0x04
//record toggle

  

//MIDI callback section
// This function will be automatically called when a MIDI message is received.
// It must be a void-returning function with the correct parameters,
// see documentation here:
// http://arduinomidilib.fortyseveneffects.com/a00022.html

//this part can be done as a CC as well
void handleNoteOn(byte channel, byte pitch, byte velocity)
{ 
  
  uint16_t velocityScale = velocity<<9; //(velocity is better 
  if (1) 
  {
    stringPlayer(pitch, velocityScale);
  }

  }


void handleNoteOff(byte channel, byte pitch, byte velocity)
{ 
}
void handleProgramChange(byte channel, byte number)//sends program change-"SCENES"
{
}
void handlePitchBend(byte channel, int bend)
{
}
//void handleStart()//using CCs for now to simplify
//{
//  SerialOne.println('P');
//}
//void handleStop()
//{
//}
//void handleContinue()
//{
//}
//void handleSystemReset()
//{
//}
////All commands are CC for simplicity sake
//void handleControlChange(byte channel, byte number, byte value)
//{
//}


void midiSetup() {
  //Midi section//
  
  midiB.begin(MIDI_CHANNEL_OMNI);
  midiB.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
  midiB.setHandleNoteOff(handleNoteOff);
 // midiB.setHandleControlChange(handleControlChange);
  midiB.setHandleProgramChange(handleProgramChange);
  midiB.setHandlePitchBend(handlePitchBend);
  midiB.turnThruOff();//no need for midi thru

}
