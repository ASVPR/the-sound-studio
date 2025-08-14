//
//  vConstants.h
//  vPi
//
//  Created by Gary Jones on 22/10/2012.
//  Copyright (c) 2012 VotanFX. All rights reserved.
//

#ifndef vPi_vConstants_h
#define vPi_vConstants_h

#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned DETECT_MODE_PEAK		= 0;
const unsigned DETECT_MODE_MS		= 1;
const unsigned DETECT_MODE_RMS		= 2;
const unsigned DETECT_MODE_NONE		= 3;

// constants for dealing with overflow or underflow
#define FLT_EPSILON_PLUS      1.192092896e-07         /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#define FLT_EPSILON_MINUS    -1.192092896e-07         /* smallest such that 1.0-FLT_EPSILON != 1.0 */
#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */


/*
 Function:	lagrpol() implements n-order Lagrange Interpolation
 
 Inputs:		double* x	Pointer to an array containing the x-coordinates of the input values
 double* y	Pointer to an array containing the y-coordinates of the input values
 int n		The order of the interpolator, this is also the length of the x,y input arrays
 double xbar	The x-coorinates whose y-value we want to interpolate
 
 Returns		The interpolated value y at xbar. xbar ideally is between the middle two values in the input array,
 but can be anywhere within the limits, which is needed for interpolating the first few or last few samples
 in a table with a fixed size.
 */
inline double lagrpol(double* x, double* y, int n, double xbar)
{
    int i,j;
    double fx=0.0;
    double l=1.0;
    for (i=0; i<n; i++)
    {
        l=1.0;
        for (j=0; j<n; j++)
		{
            if (j != i)
                l *= (xbar-x[j])/(x[i]-x[j]);
		}
		fx += l*y[i];
    }
    return (fx);
}

inline float dLinTerp(float x1, float x2, float y1, float y2, float x)
{
	float denom = x2 - x1;
	if(denom == 0)
		return y1; // should not ever happen
    
	// calculate decimal position of x
	float dx = (x - x1)/(x2 - x1);
    
	// use weighted sum method of interpolating
	float result = dx*y2 + (1-dx)*y1;
    
	return result;
    
}

inline bool normalizeBuffer(double* pInputBuffer, unsigned uBufferSize)
{
	double fMax = 0;
    
	for(unsigned j=0; j<uBufferSize; j++)
	{
		if((fabs(pInputBuffer[j])) > fMax)
			fMax = fabs(pInputBuffer[j]);
	}
    
	if(fMax > 0)
	{
		for(unsigned j=0; j<uBufferSize; j++)
			pInputBuffer[j] = pInputBuffer[j]/fMax;
	}
    
	return true;
}
#endif


// More #defines for MacOS/XCode
//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

#ifndef itoa
#define itoa(value,string,radix)  sprintf(string, "%d", value)
#endif

#ifndef ltoa
#define ltoa(value,string,radix)  sprintf(string, "%u", value)
#endif

// MAINTAINABILITY IMPROVEMENT: Audio-specific constants to replace magic numbers
// These named constants improve code readability and make it easier to change values
// consistently throughout the codebase. All audio-related hardcoded values should
// reference these constants instead of using literal numbers.
namespace AudioConstants {
    constexpr double REFERENCE_FREQUENCY_A4 = 440.0;
    constexpr double ALTERNATIVE_A4_FREQ = 432.0;  // Popular alternative tuning
    constexpr double SEMITONES_PER_OCTAVE = 12.0;
    constexpr double OCTAVE_RATIO = 2.0;
    
    // Common sample rates
    constexpr double SAMPLE_RATE_44_1K = 44100.0;
    constexpr double SAMPLE_RATE_48K = 48000.0;
    constexpr double SAMPLE_RATE_96K = 96000.0;
    
    // Common buffer sizes (powers of 2)
    constexpr int BUFFER_SIZE_64 = 64;
    constexpr int BUFFER_SIZE_128 = 128;
    constexpr int BUFFER_SIZE_256 = 256;
    constexpr int BUFFER_SIZE_512 = 512;
    constexpr int BUFFER_SIZE_1024 = 1024;
    constexpr int BUFFER_SIZE_2048 = 2048;
    constexpr int BUFFER_SIZE_4096 = 4096;
}

// Mathematical constants - modernized from macros
namespace MathConstants {
#ifdef __IOS
    constexpr double PI = 3.1415926535897932384626433832795;
#endif
    constexpr double TWO_PI = 2.0 * 3.14159265358979;
    constexpr double SQRT2_OVER_2 = 0.707106781186547524401; // same as M_SQRT1_2
}
