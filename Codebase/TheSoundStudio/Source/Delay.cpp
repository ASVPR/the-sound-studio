/*
  ==============================================================================

    Delay.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "Delay.h"
#include <algorithm>
#include <utility>

// Constructor
Delay::Delay(
             int size	// Size of delay (expressed in samples)
)
    : m_size(size), m_read(0), m_write(size - 1)
{
    // C++17: Use member initializer list and improved initialization
    m_buffer = new float[m_size];
    
    // C++17: Use std::fill instead of traditional for loop
    std::fill_n(m_buffer, m_size, 0.0f);
}

//Destructor
Delay::~Delay()
{
    delete[] m_buffer;
}

// C++17: Move constructor
Delay::Delay(Delay&& other) noexcept
    : m_size(other.m_size), m_read(other.m_read), m_write(other.m_write), m_buffer(other.m_buffer)
{
    other.m_buffer = nullptr;
    other.m_size = 0;
    other.m_read = 0;
    other.m_write = 0;
}

// C++17: Move assignment operator
Delay& Delay::operator=(Delay&& other) noexcept
{
    if (this != &other)
    {
        delete[] m_buffer;
        
        m_size = other.m_size;
        m_read = other.m_read;
        m_write = other.m_write;
        m_buffer = other.m_buffer;
        
        other.m_buffer = nullptr;
        other.m_size = 0;
        other.m_read = 0;
        other.m_write = 0;
    }
    return *this;
}

//Writes a sample into buffer
void Delay::WriteSample(
                        float sample	// Sample Value
)
{
    //write sample
    m_buffer[m_write] = sample;
    
    //update write position
    m_write = (m_write + 1) % m_size;
}

//Reads a sample from buffer
float Delay::ReadSample()
{
    // C++17: Use auto for type deduction and direct return
    const auto retVal = m_buffer[m_read];
    
    //update read position
    m_read = (m_read + 1) % m_size;
    return retVal;
}

//Process a given sample
void Delay::Process(
                    float* sample		// Address to store processed sample value
)
{
    //Write sample into delay's buffer
    WriteSample(*sample);
    
    //Update current value of sample with delayed value
    *sample = ReadSample();
}

//Reads a delayed sample from buffer
float Delay::GetDelayedSample(
                              int delay			// Delay expressed in samples
)
{
    int sampleIndex = (m_read - delay) % m_size;
    return sampleIndex >= 0 ? m_buffer[sampleIndex] : m_buffer[m_size + sampleIndex];
}
