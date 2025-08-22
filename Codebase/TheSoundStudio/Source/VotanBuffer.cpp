//
//  VotanBuffer.cpp
//  VotanSynthios
//
//  Created by nic on 04.05.13.
//  Copyright (c) 2013 com.votanfx. All rights reserved.
//

#include "VotanBuffer.h"
#include <algorithm>
#include <memory>
#include <utility>

CVotanBuffer::CVotanBuffer()
    : m_buffer(nullptr), m_allocSize(0)
{
    // C++17: Use member initializer list and nullptr
}

CVotanBuffer::~CVotanBuffer()
{
    // C++17: Use nullptr check
    if (m_buffer != nullptr)
    {
        delete[] m_buffer;
    }
}

// C++17: Move constructor
CVotanBuffer::CVotanBuffer(CVotanBuffer&& other) noexcept
    : m_buffer(other.m_buffer), m_allocSize(other.m_allocSize)
{
    other.m_buffer = nullptr;
    other.m_allocSize = 0;
}

// C++17: Move assignment operator
CVotanBuffer& CVotanBuffer::operator=(CVotanBuffer&& other) noexcept
{
    if (this != &other)
    {
        // Clean up existing resources
        if (m_buffer != nullptr)
        {
            delete[] m_buffer;
        }
        
        // Move resources
        m_buffer = other.m_buffer;
        m_allocSize = other.m_allocSize;
        
        // Reset source object
        other.m_buffer = nullptr;
        other.m_allocSize = 0;
    }
    return *this;
}

void CVotanBuffer::alloc(int size)
{
    // C++17: Use more modern comparison and nullptr
    if (m_allocSize < size)
    {
        m_allocSize = size;
        
        if (m_buffer != nullptr)
        {
            delete[] m_buffer;
        }
        
        // C++17: Consider using std::make_unique in future refactor
        m_buffer = new float[m_allocSize];
        
        // SECURITY FIX: Initialize buffer to zero to prevent reading uninitialized memory
        // Replaced unsafe memset with type-safe std::fill_n
        std::fill_n(m_buffer, m_allocSize, 0.0f);
    }
}

void CVotanBuffer::clear()
{
    // SAFETY IMPROVEMENT: Added null pointer validation to prevent crashes
    // Using std::fill instead of memset for type safety and better error checking
    if (m_buffer != nullptr && m_allocSize > 0)
    {
        std::fill(m_buffer, m_buffer + m_allocSize, 0.0f);
    }
}
