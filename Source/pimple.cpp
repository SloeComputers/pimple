//-------------------------------------------------------------------------------
// Copyright (c) 2024 John D. Haughton
// SPDX-License-Identifier: MIT
//-------------------------------------------------------------------------------

// \brief Synth for Raspberry Pi Pico (recreation of the Acorn Music 500)

#include <cstdio>

#include "Hardware/picoX7/Config.h"
#include "FilePortal.h"
#include "AMPLE/Machine.h"

#include "Synth.h"

static const unsigned DAC_FREQ         = 48000;                 //!< DAC sample rate (Hz)
static const unsigned TICK_RATE        = 400;                   //!< Voice control rate 400 Hz
static const unsigned SAMPLES_PER_TICK = DAC_FREQ / TICK_RATE;  //!< DAC buffer size (16 bit samples)
static const unsigned NUM_VOICES       = 16;
static const bool     MIDI_DEBUG       = false;


static FilePortal                    file_portal{"PiMPLE"};
static AMPLE::Machine                ample{};
static Synth<NUM_VOICES,AMPLE::Chan> synth{ample.getChannels()};


// --- Physical MIDI -----------------------------------------------------------

static hw::PhysMidi phys_midi{};


// --- USB MIDI ----------------------------------------------------------------

#if defined(HW_USB_DEVICE)

static hw::UsbFileMidi usb{0x91C0, "PiMPLE", file_portal};

extern "C" void IRQ_USBCTRL() { usb.irq(); }

#endif


// --- 16x2 LCD display --------------------------------------------------------

#if not defined(HW_LCD_NONE)

static hw::Lcd lcd{};

#endif


// --- LED ---------------------------------------------------------------------

static hw::Led led{};


// --- DAC ---------------------------------------------------------------------

static void hwTick();

static hw::Audio<SAMPLES_PER_TICK>   audio{DAC_FREQ};

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
   phys_midi.tick();
#if defined(HW_USB_DEVICE)
   usb.tick();
#endif

   synth.tick();

   led = synth.isAnyVoiceOn();

   ample.tick();
}


// --- Entry point -------------------------------------------------------------

int main()
{
   // Clear screen and cursor to home
   printf("\e[2J");
   printf("\e[1,1H");

   printf("\n");
   puts(file_portal.genREADME());
   printf("\n");

#if not defined(HW_LCD_NONE)
   lcd.move(0, 0);
   lcd.print("PiMPLE");
#endif

#if defined(HW_USB_DEVICE)
   usb.setDebug(MIDI_DEBUG);
   usb.attachInstrument(1, synth);
#endif

   phys_midi.setDebug(MIDI_DEBUG);
   phys_midi.attachInstrument(1, synth);

   audio.start();

#if defined(HW_NATIVE)
   ample.shell(/* echo */ false);
#else
   ample.shell(/* echo */ true);
#endif

   return 0;
}
