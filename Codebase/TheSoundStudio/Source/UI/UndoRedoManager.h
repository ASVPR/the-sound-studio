/*
  ==============================================================================

    UndoRedoManager.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <memory>
#include <vector>
#include <functional>

namespace TSS {
namespace UI {

//==============================================================================
/**
    Base class for undoable actions.
    Inherit from this to create custom actions.
*/
class UndoableAction
{
public:
    virtual ~UndoableAction() = default;

    /**
        Perform the action.
        @return true if successful
    */
    virtual bool perform() = 0;

    /**
        Undo the action.
        @return true if successful
    */
    virtual bool undo() = 0;

    /**
        Get a description of this action for display.
    */
    virtual juce::String getDescription() const = 0;

    /**
        Get the size in bytes of this action's data.
        Used for memory management.
    */
    virtual size_t getSizeInBytes() const { return sizeof(*this); }

    /**
        Check if this action can be merged with another.
        Useful for combining consecutive similar actions (e.g., typing).
    */
    virtual bool canMergeWith(const UndoableAction* other) const
    {
        (void)other;
        return false;
    }

    /**
        Merge another action into this one.
        Only called if canMergeWith returns true.
    */
    virtual void mergeWith(const UndoableAction* other)
    {
        (void)other;
    }
};

//==============================================================================
/**
    Action group that executes multiple actions as a single unit.
*/
class ActionGroup : public UndoableAction
{
public:
    ActionGroup(const juce::String& desc) : description(desc) {}

    void addAction(std::unique_ptr<UndoableAction> action)
    {
        actions.push_back(std::move(action));
    }

    bool perform() override
    {
        for (auto& action : actions)
        {
            if (!action->perform())
                return false;
        }
        return true;
    }

    bool undo() override
    {
        // Undo in reverse order
        for (auto it = actions.rbegin(); it != actions.rend(); ++it)
        {
            if (!(*it)->undo())
                return false;
        }
        return true;
    }

    juce::String getDescription() const override
    {
        return description;
    }

    size_t getSizeInBytes() const override
    {
        size_t total = sizeof(*this);
        for (const auto& action : actions)
            total += action->getSizeInBytes();
        return total;
    }

private:
    juce::String description;
    std::vector<std::unique_ptr<UndoableAction>> actions;
};

//==============================================================================
/**
    Manages undo/redo history with memory limits and grouping.
*/
class UndoRedoManager
{
public:
    UndoRedoManager(size_t maxMemoryBytes = 10 * 1024 * 1024)  // 10MB default
        : maxMemory(maxMemoryBytes)
        , currentMemory(0)
        , currentPosition(-1)
        , groupDepth(0)
    {
    }

    ~UndoRedoManager() = default;

    //==========================================================================
    // Basic operations

    /**
        Execute and store an action in the history.
        @param action The action to perform
        @return true if successful
    */
    bool perform(std::unique_ptr<UndoableAction> action)
    {
        if (!action)
            return false;

        // Execute the action
        if (!action->perform())
            return false;

        // If we're in a group, add to current group
        if (groupDepth > 0 && currentGroup)
        {
            currentGroup->addAction(std::move(action));
            return true;
        }

        // Try to merge with last action
        if (currentPosition >= 0 &&
            currentPosition < static_cast<int>(history.size()) &&
            history[currentPosition]->canMergeWith(action.get()))
        {
            history[currentPosition]->mergeWith(action.get());
            notifyListeners();
            return true;
        }

        // Clear any actions after current position
        if (currentPosition < static_cast<int>(history.size()) - 1)
        {
            for (size_t i = currentPosition + 1; i < history.size(); ++i)
                currentMemory -= history[i]->getSizeInBytes();
            history.erase(history.begin() + currentPosition + 1, history.end());
        }

        // Add new action
        size_t actionSize = action->getSizeInBytes();
        currentMemory += actionSize;
        history.push_back(std::move(action));
        currentPosition = static_cast<int>(history.size()) - 1;

        // Trim history if over memory limit
        trimHistory();
        notifyListeners();
        return true;
    }

    /**
        Undo the last action.
        @return true if successful
    */
    bool undo()
    {
        if (!canUndo())
            return false;

        if (history[currentPosition]->undo())
        {
            --currentPosition;
            notifyListeners();
            return true;
        }

        return false;
    }

    /**
        Redo the next action.
        @return true if successful
    */
    bool redo()
    {
        if (!canRedo())
            return false;

        if (history[currentPosition + 1]->perform())
        {
            ++currentPosition;
            notifyListeners();
            return true;
        }

        return false;
    }

    /**
        Clear all history.
    */
    void clearHistory()
    {
        history.clear();
        currentPosition = -1;
        currentMemory = 0;
        notifyListeners();
    }

    //==========================================================================
    // Action grouping

    /**
        Begin a group of actions that will be treated as a single unit.
    */
    void beginActionGroup(const juce::String& description)
    {
        if (groupDepth == 0)
        {
            currentGroup = std::make_unique<ActionGroup>(description);
        }
        ++groupDepth;
    }

    /**
        End the current action group.
    */
    void endActionGroup()
    {
        if (groupDepth == 0)
            return;

        --groupDepth;

        if (groupDepth == 0 && currentGroup)
        {
            perform(std::move(currentGroup));
            currentGroup = nullptr;
        }
    }

    //==========================================================================
    // Query methods

    bool canUndo() const
    {
        return currentPosition >= 0;
    }

    bool canRedo() const
    {
        return currentPosition < static_cast<int>(history.size()) - 1;
    }

    juce::String getUndoDescription() const
    {
        if (!canUndo())
            return juce::String();
        return history[currentPosition]->getDescription();
    }

    juce::String getRedoDescription() const
    {
        if (!canRedo())
            return juce::String();
        return history[currentPosition + 1]->getDescription();
    }

    int getNumActionsInHistory() const
    {
        return static_cast<int>(history.size());
    }

    int getCurrentPosition() const
    {
        return currentPosition;
    }

    size_t getCurrentMemoryUsage() const
    {
        return currentMemory;
    }

    //==========================================================================
    // Listeners

    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void undoRedoStateChanged() = 0;
    };

    void addListener(Listener* listener)
    {
        listeners.add(listener);
    }

    void removeListener(Listener* listener)
    {
        listeners.remove(listener);
    }

private:
    void trimHistory()
    {
        // Remove oldest actions until under memory limit
        while (currentMemory > maxMemory && !history.empty())
        {
            currentMemory -= history[0]->getSizeInBytes();
            history.erase(history.begin());
            --currentPosition;
        }

        // Ensure position is valid
        if (currentPosition < -1)
            currentPosition = -1;
    }

    void notifyListeners()
    {
        listeners.call([](Listener& l) { l.undoRedoStateChanged(); });
    }

    std::vector<std::unique_ptr<UndoableAction>> history;
    int currentPosition;
    size_t maxMemory;
    size_t currentMemory;
    int groupDepth;
    std::unique_ptr<ActionGroup> currentGroup;
    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UndoRedoManager)
};

//==============================================================================
/**
    Helper RAII class for action grouping.
*/
class ScopedActionGroup
{
public:
    ScopedActionGroup(UndoRedoManager& mgr, const juce::String& description)
        : manager(mgr)
    {
        manager.beginActionGroup(description);
    }

    ~ScopedActionGroup()
    {
        manager.endActionGroup();
    }

private:
    UndoRedoManager& manager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedActionGroup)
};

//==============================================================================
/**
    Example simple action using lambdas.
*/
class LambdaAction : public UndoableAction
{
public:
    LambdaAction(const juce::String& desc,
                std::function<bool()> doFunc,
                std::function<bool()> undoFunc)
        : description(desc)
        , doFunction(std::move(doFunc))
        , undoFunction(std::move(undoFunc))
    {
    }

    bool perform() override
    {
        return doFunction ? doFunction() : false;
    }

    bool undo() override
    {
        return undoFunction ? undoFunction() : false;
    }

    juce::String getDescription() const override
    {
        return description;
    }

private:
    juce::String description;
    std::function<bool()> doFunction;
    std::function<bool()> undoFunction;
};

//==============================================================================
/**
    Example action for value changes with type safety.
*/
template<typename T>
class ValueChangeAction : public UndoableAction
{
public:
    ValueChangeAction(const juce::String& desc,
                     T& valueRef,
                     const T& newValue)
        : description(desc)
        , value(valueRef)
        , oldValue(valueRef)
        , newValue(newValue)
    {
    }

    bool perform() override
    {
        value = newValue;
        return true;
    }

    bool undo() override
    {
        value = oldValue;
        return true;
    }

    juce::String getDescription() const override
    {
        return description;
    }

private:
    juce::String description;
    T& value;
    T oldValue;
    T newValue;
};

} // namespace UI
} // namespace TSS
