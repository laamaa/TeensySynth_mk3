#include "synth_plaits_f32.h"
#include "plaits/dsp/dsp.h"
#include "plaits/dsp/voice.h"

using namespace plaits;
using namespace stmlib;

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
