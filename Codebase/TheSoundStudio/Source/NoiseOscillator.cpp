/*
  ==============================================================================

    NoiseOscillator.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "NoiseOscillator.h"

const float PinkNoise::A[] = { 0.02109238, 0.07113478, 0.68873558 }; // rescaled by (1+P)/(1-P)
const float PinkNoise::P[] = { 0.3190,  0.7756,  0.9613  };
