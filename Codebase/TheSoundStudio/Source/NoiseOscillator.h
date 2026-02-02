/*
  ==============================================================================

    NoiseOscillator.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once


#define PINK_NOISE_NUM_STAGES 3

#include <cstdlib>
#include <ctime>
#include <iostream>



class PinkNoise {
public:
    PinkNoise()
    {
        srand ( static_cast<unsigned int>(time(nullptr)) );
        clear();
    }
    
    void clear() {
        for( size_t i=0; i< PINK_NOISE_NUM_STAGES; i++ )
            state[ i ] = 0.0;
    }
    
    float tick() {
        static const float RMI2 = 2.0 / float(RAND_MAX); // + 1.0; // change for range [0,1)
        static const float offset = A[0] + A[1] + A[2];
        
        // unrolled loop
        float temp = float( rand() );
        state[0] = P[0] * (state[0] - temp) + temp;
        temp = float( rand() );
        state[1] = P[1] * (state[1] - temp) + temp;
        temp = float( rand() );
        state[2] = P[2] * (state[2] - temp) + temp;
        return ( A[0]*state[0] + A[1]*state[1] + A[2]*state[2] )*RMI2 - offset;
    }
    
protected:
    float state[ PINK_NOISE_NUM_STAGES ];
    static const float A[ PINK_NOISE_NUM_STAGES ];
    static const float P[ PINK_NOISE_NUM_STAGES ];
};


class WhiteNoise {
public:
    WhiteNoise() {
        //srand ( time(NULL) ); // initialize random generator
        clear();
    }
    
    void clear()
    {
        g_fScale    = 2.0f / 0xffffffff;
        g_x1        = 0x67452301;
        g_x2        = 0xefcdab89;
    }
    
    float tick()
    {
        float _fLevel = 1.0;
        _fLevel *= g_fScale;
        g_x1 ^= g_x2;
        float outsamp = g_x2 * _fLevel;
        g_x2 += g_x1;
        
        return outsamp;
    }
    
protected:
    
    float g_fScale  = 2.0f / 0xffffffff;
    int g_x1        = 0x67452301;
    int g_x2        = 0xefcdab89;

};

/*
     void processWhiteNoise(AudioBuffer<float>& buffer)
     {
         float fNoise = 0.0;
     
 //#if defined _WINDOWS || defined _WINDLL
 //    // fNoise is 0 -> 32767.0
 //    fNoise = (float)rand();
 //
 //    // normalize and make bipolar
 //    fNoise = 2.0*(fNoise/32767.0) - 1.0;
 //#else
       // fNoise is 0 -> ARC4RANDOMMAX
 //    fNoise = (float)arc4random(); fix Linux
 //    fNoise = 0; // again fix for linux compile..
     
         for (int sample = 0; sample < buffer.getNumSamples(); sample++)
         {
             // normalize and make bipolar
             fNoise = 2.0*(fNoise/ARC4RANDOMMAX) - 1.0;
             
             buffer.getWritePointer(0)[sample] = fNoise;
             
             if (buffer.getNumChannels() == 2) buffer.getWritePointer(1)[sample] = fNoise;
         }
     }
     
     unsigned int pinkNoiseIterator;
     
     void processPinkNoise(AudioBuffer<float>& buffer)
     {
         for (int sample = 0; sample < buffer.getNumSamples(); sample++)
         {
             // get the bits
             UINT b0     = EXTRACT_BITS(pinkNoiseIterator, 1, 1); // 1 = b0 is FIRST bit from right
             UINT b1     = EXTRACT_BITS(pinkNoiseIterator, 2, 1); // 1 = b1 is SECOND bit from right
             UINT b27    = EXTRACT_BITS(pinkNoiseIterator, 28, 1); // 28 = b27 is 28th bit from right
             UINT b28    = EXTRACT_BITS(pinkNoiseIterator, 29, 1); // 29 = b28 is 29th bit from right

             // form the XOR
             UINT b31 = b0^b1^b27^b28;

             // form the mask to OR with the register to load b31
             if(b31 == 1)
                 b31 = 0x10000000;

             // shift one bit to right
             pinkNoiseIterator >>= 1;

             // set the b31 bit
             pinkNoiseIterator |= b31;

             // convert the output into a floating point number, scaled by experimentation
             // to a range of o to +2.0
             float fOut = (float)(pinkNoiseIterator)/((pow((float)2.0,(float)32.0))/16.0);

             // shift down to form a result from -1.0 to +1.0
             fOut -= 1.0;
             
             buffer.getWritePointer(0)[sample] = fOut;
             
             if (buffer.getNumChannels() == 2) buffer.getWritePointer(1)[sample] = fOut;
         }
     }
 */
