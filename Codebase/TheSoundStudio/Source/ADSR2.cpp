/*
  ==============================================================================

    ADSR2.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ADSR2.h"
#include <math.h>


ADSR2::ADSR2(double newSampleRate)
{
    sample_rate = newSampleRate;
    
    reset();
    setTargetRatioA(100.0);
    setTargetRatioDR(1.0);
    
    attackParam     = 0.f;
    decayParam      = 10.f;
    sustainParam    = 1.f;
    releaseParam    = 100.f;
}

ADSR2::~ADSR2(void) { }

void ADSR2::processBuffer(float * envbuf, int numSamples)
{
    for (int s = 0; s < numSamples; s++)
    {
        envbuf[s] = process();
    }
}

// below should all be inline, but doesnt compile
float ADSR2::process() {
    switch (state) {
        case env_idle:
            break;
        case env_attack:
            output = attackBase + output * attackCoef;
            if (output >= 1.0) {
                output = 1.0;
                state = env_decay;
            }
            break;
        case env_decay:
            output = decayBase + output * decayCoef;
            if (output <= sustainLevel) {
                output = sustainLevel;
                state = env_sustain;
            }
            break;
        case env_sustain:
            break;
        case env_release:
            output = releaseBase + output * releaseCoef;
            if (output <= 0.0) {
                output = 0.0;
                state = env_idle;
            }
    }
    return output;
}



void ADSR2::gate(int gate)
{
    if (gate)
        state = env_attack;
    else if (state != env_idle)
        state = env_release;
}

int ADSR2::getState()
{
    return state;
}

void ADSR2::reset()
{
    state = env_idle;
    output = 0.0;
}

float ADSR2::getOutput() {
    return output;
}

void ADSR2::setAttackRate(float rate) {
    attackRate = rate * sample_rate / 1000;
    attackCoef = calcCoef(rate, targetRatioA);
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
   // printf("Attack = %f", attackRate);
}
// only change targetratio D or R .***
void ADSR2::setDecayRate(float rate) {
    decayRate = rate * sample_rate / 1000;
    decayCoef = calcCoef(rate, targetRatioDR);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
   // printf("Decay = %f", decayRate);
}

void ADSR2::setReleaseRate(float rate) {
    releaseRate = rate;
    releaseCoef = calcCoef(rate, targetRatioDR);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
   // printf("Release = %f", releaseRate);
}

float ADSR2::calcCoef(float rate, float targetRatio) {
    return exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

void ADSR2::setSustainLevel(float level) {
    sustainLevel = level;
   // decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
  //  printf("sustain = %f", sustainLevel);
}

void ADSR2::setTargetRatioA(float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioA = targetRatio;
    attackBase = (1.0 + targetRatioA) * (1.0 - attackCoef);
}

void ADSR2::setTargetRatioDR(float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioDR = targetRatio;
    decayBase = (sustainLevel - targetRatioDR) * (1.0 - decayCoef);
    releaseBase = -targetRatioDR * (1.0 - releaseCoef);
}

void ADSR2::setTargetRatioD(float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioD = targetRatio;
    decayBase = (sustainLevel - targetRatioD) * (1.0 - decayCoef);
}

void ADSR2::setTargetRatioR(float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    targetRatioR = targetRatio;
    releaseBase = -targetRatioR * (1.0 - releaseCoef);
}

void ADSR2::setNewSamplerate(float newSampleRate)
{
    sample_rate = newSampleRate;
    
    SetAttack(attackParam);
    SetDecay(decayParam);
    SetSustain(sustainParam);
    SetRelease(releaseParam);
}

void ADSR2::setOversampleRate(int newRate)
{
    
}

float ADSR2::convertToLog(float value, float minVal, float maxVal)
{
    return powf(10.f, value * (log10f(maxVal)-log10f(minVal))+log10f(minVal));
}

void ADSR2::SetAttack(float value)
{
    attackParam = value;
    setAttackRate(attackParam * sample_rate / 1000);
}

void ADSR2::SetDecay(float value)
{
    decayParam = value;
    setDecayRate(decayParam * sample_rate / 1000);
}

void ADSR2::SetSustain(float value)
{
    sustainParam = value;
    setSustainLevel(value);
}

void ADSR2::SetRelease(float value)
{
    releaseParam = value;
    setReleaseRate(releaseParam * sample_rate / 1000);
}
