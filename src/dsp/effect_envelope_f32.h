#ifndef EFFECTENVELOPEF32H
#define EFFECTENVELOPEF32H

/* Exponential envelope for Chipaudette's Openaudio Library
 * based on https://www.musicdsp.org/en/latest/Synthesis/189-fast-exponential-envelope-generator.html
 */

#include <Arduino.h>
#include "AudioStream_F32.h"

#define FXDEBUG 0

class AudioEffectEnvelope_F32 : public AudioStream_F32
{
public:
    AudioEffectEnvelope_F32() : AudioStream_F32(1, inputQueueArray_f32)
    {
        setAttack(500.0f);
        setDecay(500.0f);
        setSustain(0.5f);
        setRelease(300.0f);
    }

    virtual void update(void);

    void setAttack(float attack)
    {
        if (attack < 0)
            attack = 0;
        attackMillis = attack;
        attackCount = millisToCount(attack);
    }

    void setDecay(float decay)
    {
        if (decay < 0)
            decay = 0;
        decayMillis = decay;
        decayCount = millisToCount(decay);
    }

    void setSustain(float sustain)
    {
        if (sustain < prettyLowValue)
            sustain = prettyLowValue;
        else if (sustain > 1.0f)
            sustain = 1.0f;
        sustainLevel = sustain;
    }

    void setRelease(float release)
    {
        if (release < 0.5f)
            release = 0.5f;
        releaseMillis = release;
        releaseCount = millisToCount(release);
    }

    bool bypass = false;

    void noteOn();
    void noteOff();

private:
    const float samplesPerMs = (AUDIO_SAMPLE_RATE_EXACT / 1000.0);
    const float prettyLowValue = 0.0001f; // The coeff calculation formula must have something else than 0.0f as levelEnd.
    audio_block_f32_t *inputQueueArray_f32[1];

    // Possible envelope states
    enum envelopeState
    {
        envIdle = 0,
        envAttack,
        envDecay,
        envSustain,
        envRelease
    };

    inline float calculateCoeff(float levelEnd, float levelBegin, float duration)
    {
        return (log(levelEnd) - log(levelBegin)) / (duration * samplesPerMs);
    }

    // Return the amount of samples in a time period (in milliseconds)
    uint16_t millisToCount(float millis) { return (millis * samplesPerMs); }

    // Populate the array that contains the gain information for the current sample block
    void calculateGains(uint16_t blockSize);

    // Current envelope state
    envelopeState state = envIdle;

    // Envelope durations in samples
    uint16_t count, attackCount, decayCount, releaseCount;

    // Envelope durations in milliseconds
    float attackMillis, decayMillis, releaseMillis;

    // Sustain level multiplier
    float sustainLevel;

    // Multiplier for signal gain
    float coeff;

    // Current relative volume level
    float currentLevel;

    // Relative levels of the envelope
    float levelBegin, levelEnd;

    // Array containing gain multipliers for signal processing
    float gainArray[AUDIO_BLOCK_SAMPLES];

#if FXDEBUG > 0
    elapsedMillis debugTimer;
#endif
};

#endif