/*
  ==============================================================================

 scoped_toggle_atomic.h
    Created: 2 Feb 2022 12:06:47pm
    Author:  Javier Andoni Aizpuru Rodríguez

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
