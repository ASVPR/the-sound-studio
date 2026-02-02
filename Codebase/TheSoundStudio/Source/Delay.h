/*
  ==============================================================================

    Delay.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/


#pragma once
//
//  Delay.h
//  AudioVitamins
//
//  Created by Gary Jones on 28/12/2015.
//
//

#ifndef __AudioVitamins__Delay__
#define __AudioVitamins__Delay__

#include <stdio.h>
#include <memory>

// Audio delay class
class Delay
{
    
public:
    
    // Constructor
    Delay(
          int size	// Size of delay (expressed in samples)
    );
    
    // C++17: Move constructor and assignment
    Delay(Delay&& other) noexcept;
    Delay& operator=(Delay&& other) noexcept;
    
    // Delete copy to prevent expensive buffer copying
    Delay(const Delay&) = delete;
    Delay& operator=(const Delay&) = delete;
    
    //Destructor
    ~Delay();
    
    //Process one sample
    void Process(
                 float* sample			// Address to store processed sample value
				);
    
    //Get sample delayed
    float GetDelayedSample(
                           int delay	// Delay expressed in samples
    );
    
    //Reads a sample
    float ReadSample();
    
    //Writes a sample
    void WriteSample(float sample);
    
    // C++17: Add constexpr getter methods
    constexpr int size() const noexcept { return m_size; }
    
    int m_size;			// Size of delay in samples
    
private:
    
    
    int m_read;			// Read position
    int m_write;		// Write position, always will be (read + size) % size
    float* m_buffer;	// Samples buffer
    
};


#endif /* defined(__AudioVitamins__Delay__) */
