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
#define BUFFER_MAX 1500

class AudioSynthKarplusStrongMod : public AudioStream
{
public:
	AudioSynthKarplusStrongMod() : AudioStream(0, NULL) {
		state = 0;
	}
	void noteOn(float frequency, uint16_t velocity )
	{
		if (velocity > 65535) {
			velocity = 0;
		} else if (velocity <= 1) {
			noteOff();
			return;
		}
		magnitude = velocity ;
		decay = 32768;
		int len = (AUDIO_SAMPLE_RATE_EXACT / frequency) + 0.5f;
		if (len > BUFFER_MAX) len =BUFFER_MAX;
		bufferLen = len;
		bufferIndex = 0;
		state = 1;
	}
	void noteOff() {
		state = 0;
	}
	virtual void update(void);
private:
	uint8_t  state;     // 0=steady output, 1=begin on next update, 2=playing
	uint16_t bufferLen;
	uint16_t bufferIndex;
	int32_t  magnitude; // current output
	int16_t decay;
	int16_t scaleMult;
	static uint32_t seed;  // must start at 1
	int16_t buffer[BUFFER_MAX]; // TODO: dynamically use audio memory blocks
};

#endif
