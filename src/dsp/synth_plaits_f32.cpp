#include "synth_plaits_f32.h"
#include "plaits/dsp/dsp.h"
#include "plaits/dsp/voice.h"

using namespace plaits;
using namespace stmlib;

void AudioSynthPlaits_F32::initPlaits() {
        static char DMAMEM shared_buffer[16384]; //Store the synth buffers in the beginning of RAM2
        BufferAllocator allocator(shared_buffer, 16384);
        voice.Init(&allocator);
        patch.engine = 0;
        patch.note = 48.0f;
        modulations.note = 0.0f;
        modulations.engine = 0.0f;
        modulations.frequency = 0.0f;
        modulations.note = 0.0f;
        modulations.harmonics = 0.0f;
        modulations.morph = 0.0;
        modulations.level = 0.0f;
        modulations.trigger = 0.0f;
        modulations.frequency_patched = false;
        modulations.timbre_patched = false;
        modulations.morph_patched = true;
        modulations.trigger_patched = true;
        modulations.level_patched = false;
}

void AudioSynthPlaits_F32::update(void)
{
    audio_block_f32_t *blockOutMain;
    audio_block_f32_t *blockOutAux;

    blockOutMain = AudioStream_F32::allocate_f32();
    blockOutAux = AudioStream_F32::allocate_f32();
    if (blockOutMain == NULL || blockOutAux == NULL)
        return;

    Voice::Frame out[AUDIO_BLOCK_SAMPLES];
    voice.Render(patch, modulations, out, AUDIO_BLOCK_SAMPLES);
    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        blockOutMain->data[i] = out[i].out;
        blockOutAux->data[i] = out[i].aux;
    }
    AudioStream_F32::transmit(blockOutMain, 0);
    AudioStream_F32::transmit(blockOutAux, 1);
    AudioStream_F32::release(blockOutMain);
    AudioStream_F32::release(blockOutAux);
    return;
}
