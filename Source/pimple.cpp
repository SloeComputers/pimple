//------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//------------------------------------------------------------------------------

// \brief Synth for Raspberry Pi Pico (recreation of the Acorn Music 500)

#include <cstdio>

#include "hw/hw.h"
#include "AMPLE/Shell.h"

#include "Synth.h"
#include "Voice.h"


static const unsigned DAC_FREQ         = 48000;                 //!< DAC sample rate (Hz)
static const unsigned TICK_RATE        = 400;                   //!< Voice control rate 400 Hz
static const unsigned SAMPLES_PER_TICK = DAC_FREQ / TICK_RATE;  //!< DAC buffer size (16 bit samples)
static const unsigned NUM_VOICES       = 16;
static const bool     MIDI_DEBUG       = false;


static Synth<NUM_VOICES,Voice<DAC_FREQ>> synth {};
static hw::MidiIn                        midi_in {synth, MIDI_DEBUG};
static hw::Led                           led {};
static hw::Audio<SAMPLES_PER_TICK>       audio {DAC_FREQ};


#if defined(HW_MIDI_USB_DEVICE)

static hw::MidiUSBDevice midi_usb {synth, 0x917E, "PIMPLE", MIDI_DEBUG};

extern "C" void IRQ_USBCTRL() { midi_usb.usb.irq(); }

#endif

static void hwTick();

#if defined(HW_DAC_I2S)

MTL_AUDIO_ATTACH_IRQ_0(audio);

void MTL::Audio::getSamples(uint32_t* buffer, unsigned n)
{
   for(unsigned i = 0; i < n; i++)
   {
      int16_t sample = synth.getSample();
      buffer[i] = (sample << 16) | (sample & 0xFFFF);
   }

   hwTick();
}

#elif defined(HW_DAC_PWM)

MTL_AUDIO_ATTACH_IRQ_0(audio);

void MTL::Audio::getSamples(uint32_t* buffer, unsigned n)
{
   for(unsigned i = 0; i < n; i++)
   {
      int16_t sample = synth.getSample();
      buffer[i] = audio.packSamples(sample, 0);
   }

   hwTick();
}

#elif defined(HW_DAC_NATIVE)

template<>
void hw::Audio<SAMPLES_PER_TICK>::getSamples(int16_t* buffer, unsigned n)
{
   (void) MIDI_DEBUG; // XXX avoid an unrelated warning

   for(unsigned i = 0; i < n; i += 2)
   {
      int16_t sample = synth.getSample();
      buffer[i + 1] = buffer[i] = sample;
   }

   hwTick();
}

#endif


static void hwTick()
{
   midi_in.tick();
#if defined(HW_MIDI_USB_DEVICE)
   midi_usb.tick();
#endif

   synth.tick();

   led = synth.isAnyVoiceOn();
}


int main()
{
   // Clear screen and cursor to home
   printf("\e[2J");
   printf("\e[1,1H");

   printf("\n");
   printf("Program  : PIMPLE (%s)\n", HW_DESCR);
   printf("Author   : Copyright (c) 2024 John D. Haughton\n");
   printf("License  : MIT\n");
   printf("Version  : %s\n", PLT_VERSION);
   printf("Commit   : %s\n", PLT_COMMIT);
   printf("Built    : %s %s\n", __TIME__, __DATE__);
   printf("Compiler : %s\n", __VERSION__);
   printf("\n");

#if not defined(HW_LCD_NONE)
   hw::Lcd lcd{};

   lcd.move(0, 0);
   lcd.print("PIMPLE");
#endif

   audio.start();

   AMPLE::Shell shell{};

#if defined(HW_NATIVE)
   shell.run(/* echo */ false);
#else
   shell.run(/* echo */ true);
#endif

   return 0;
}
