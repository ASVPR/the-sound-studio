/*
  ==============================================================================

    VotanBuffer.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/


#pragma once
//
//  VotanBuffer.h
//  VotanSynthios
//
//  Created by nic on 04.05.13.
//  Copyright (c) 2013 com.votanfx. All rights reserved.
//

#ifndef __VotanSynthios__VotanBuffer__
#define __VotanSynthios__VotanBuffer__

#include <iostream>
#include <memory>

class CVotanBuffer
{
public:
    CVotanBuffer();
    virtual ~CVotanBuffer();
    
    // C++17: Move constructor and assignment operator
    CVotanBuffer(CVotanBuffer&& other) noexcept;
    CVotanBuffer& operator=(CVotanBuffer&& other) noexcept;
    
    // Delete copy constructor and assignment to prevent accidental copying of large buffers
    CVotanBuffer(const CVotanBuffer&) = delete;
    CVotanBuffer& operator=(const CVotanBuffer&) = delete;
    
public:
    void alloc(int size);
    void clear();
    
    // C++17: Add constexpr getter
    constexpr int size() const noexcept { return m_allocSize; }
    
    // C++17: Add safer access methods
    float* data() noexcept { return m_buffer; }
    const float* data() const noexcept { return m_buffer; }
    
public:
    float* m_buffer;
    
private:
    int m_allocSize;
    
};

#endif /* defined(__VotanSynthios__VotanBuffer__) */
