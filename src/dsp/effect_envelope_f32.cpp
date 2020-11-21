#include <Arduino.h>
#include "effect_envelope_f32.h"
#include "arm_math.h"

void AudioEffectEnvelope_F32::noteOn()
{
    if (!enabled)
        return;
    __disable_irq();
    if (attackCount > 0)
    {
#if FXDEBUG > 0
        Serial.println(F("Envelope: attack"));
#endif
        state = envAttack;
        count = attackCount;
        currentLevel = prettyLowValue;
        coeff = calculateCoeff(1.0f, prettyLowValue, attackMillis);
    }
    else
    {
        // If attack is 0.0, just skip attack
#if FXDEBUG > 0
        Serial.println(F("Envelope: decay"));
#endif
        state = envDecay;
        count = decayCount;
        currentLevel = 1.0f;
        coeff = calculateCoeff(sustainLevel, 1.0f, decayMillis);
    }
    __enable_irq();
}

void AudioEffectEnvelope_F32::noteOff()
{
    if (!enabled || state == envelopeState::envIdle)
        return;
    __disable_irq();
    // Sustain is done, go to release
#if FXDEBUG > 0
    Serial.println(F("Envelope: release"));
#endif
    state = envRelease;
    count = releaseCount;
    coeff = calculateCoeff(prettyLowValue, currentLevel, releaseMillis);
    __enable_irq();
}

void AudioEffectEnvelope_F32::update()
{
    __disable_irq();
    audio_block_f32_t *block;

    block = AudioStream_F32::receiveWritable_f32();
    if (!block)
        return;

    if (this->enabled == true)
    {
        if (state == envIdle)
        {
            release(block);
            return;
        }

        calculateGains(AUDIO_BLOCK_SAMPLES);

        arm_mult_f32(block->data, gainArray, block->data, AUDIO_BLOCK_SAMPLES);
    }
    AudioStream_F32::transmit(block);
    AudioStream_F32::release(block);
}

void AudioEffectEnvelope_F32::calculateGains(uint16_t blockSize)
{
    for (int i = 0; i < blockSize; i++)
    {
        // Envelope state changes when count reaches zero
        if (count == 0)
        {
            switch (state)
            {
            case envAttack:
                // Attack is done, go to decay
#if FXDEBUG > 0
                Serial.println(F("Envelope: decay"));
#endif
                state = envDecay;
                count = decayCount;
                currentLevel = 1.0f;
                coeff = calculateCoeff(sustainLevel, 1.0f, decayMillis);
                break;
            case envDecay:
                // Decay is done, go to sustain
#if FXDEBUG > 0
                Serial.println(F("Envelope: sustain"));
#endif
                state = envSustain;
                count = 0xFFFF;
                currentLevel = sustainLevel;
                coeff = 0.0f;
                break;
            case envRelease:
                // Envelope complete, go idle
#if FXDEBUG > 0
                Serial.println(F("Envelope: idle"));
#endif
                state = envIdle;
                count = 0xFFFF;
                currentLevel = prettyLowValue;
                coeff = 0.0f;
                break;
            default:
                return;
                break;
            }
        }

        currentLevel += coeff * currentLevel;
        gainArray[i] = currentLevel;
#if FXDEBUG > 0
        if (debugTimer > 200)
        {
            Serial.printf("count %d, state %d, coef %.2f, lvl %.2f\n", count, state, coeff, currentLevel);
            debugTimer = 0;
        }
#endif
        count--;
    }
}