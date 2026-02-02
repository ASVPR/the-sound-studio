/*
  ==============================================================================

    ADSR2.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/
#pragma once
#include "math.h"

class ADSR2 {
public:
	ADSR2(double newSampleRate);
	~ADSR2(void);
    
	float process(void);
    float getOutput(void);
    void processBuffer(float * envbuf, int numSamples);
    int getState(void);
	void gate(int on);

    void reset(void);

    enum envState {
        env_idle = 0,
        env_attack,
        env_decay,
        env_sustain,
        env_release
    };
    
    float over_sample_factor = 1;
    double sample_rate = 44100;
    
    void setNewSamplerate(float newSampleRate);
    void setOversampleRate(int newRate);
    
    float convertToLog(float value, float minVal, float maxVal);
    
    void SetAttack(float value);
    void SetDecay(float value);
    void SetSustain(float value);
    void SetRelease(float value);

protected:
    void setAttackRate(float rate);
    void setDecayRate(float rate);
    void setReleaseRate(float rate);
    void setSustainLevel(float level);
    void setTargetRatioA(float targetRatio);
    void setTargetRatioDR(float targetRatio);
    void setTargetRatioR(float targetRatio);
    void setTargetRatioD(float targetRatio);
    
	int state;
	float output;
	float attackRate;
	float decayRate;
	float releaseRate;
	float attackCoef;
	float decayCoef;
	float releaseCoef;
	float sustainLevel;
    float targetRatioA;
    float targetRatioDR;
    float targetRatioD;
    float targetRatioR;
    float attackBase;
    float decayBase;
    float releaseBase;
    
    float attackParam;
    float decayParam;
    float sustainParam;
    float releaseParam;
 
    float calcCoef(float rate, float targetRatio);
};
