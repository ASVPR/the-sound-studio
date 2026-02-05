/*
  ==============================================================================

    scoped_toggle_atomic.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

namespace audio_devs
{

template<typename Type>
class scoped_toggle_atomic
{
public:
    scoped_toggle_atomic(scoped_toggle_atomic&) = delete;
    scoped_toggle_atomic(scoped_toggle_atomic&&) = delete;
    scoped_toggle_atomic operator=(scoped_toggle_atomic&) = delete;
    scoped_toggle_atomic operator=(scoped_toggle_atomic&&) = delete;

    scoped_toggle_atomic(std::atomic<Type>& ref_object,
                       Type initialValue,
                       Type onDestroyValue) : object(ref_object)
    {
        init(initialValue, onDestroyValue);
    };

    scoped_toggle_atomic(std::atomic<Type>& ref_object,
                       Type initialValue,
                       Type onDestroyValue,
                       std::memory_order initial_memory_order,
                       std::memory_order destroy_memory_order) : object(ref_object)
    {
        init(initialValue, onDestroyValue, initial_memory_order, destroy_memory_order);
    };

    ~scoped_toggle_atomic()
    {
        object.store(onDestroy, onDestroyOrder);
    };


    void init(Type initialValue,
              Type onDestroyValue,
              std::memory_order initial_memory_order = std::memory_order_release,
              std::memory_order destroy_memory_order = std::memory_order_release)
    {
        object.store(initialValue, initial_memory_order);
        onDestroyOrder = destroy_memory_order;
        onDestroy = onDestroyValue;
    }

private:

    std::atomic<Type>& object;

    Type onDestroy;

    std::memory_order onDestroyOrder;
};

}
