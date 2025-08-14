/*
  ==============================================================================

    NoiseOscillator.cpp
    Created: 16 Oct 2020 8:56:30am
    Author:  Gary Jones

  ==============================================================================
*/

#include "NoiseOscillator.h"

const float PinkNoise::A[] = { 0.02109238, 0.07113478, 0.68873558 }; // rescaled by (1+P)/(1-P)
const float PinkNoise::P[] = { 0.3190,  0.7756,  0.9613  };
