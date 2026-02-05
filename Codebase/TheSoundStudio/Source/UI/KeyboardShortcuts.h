/*
  ==============================================================================

    KeyboardShortcuts.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include <functional>

namespace TSS {
namespace UI {

//==============================================================================
/**
    Central keyboard shortcuts system for the application.
    Provides command registration, execution, and customizable key bindings.
*/
class KeyboardShortcuts
{
public:
    //==========================================================================
    // Command IDs
    enum class CommandID
    {
        // File operations
        NewProject = 1000,
        OpenProject,
        SaveProject,
        SaveProjectAs,
        CloseProject,

        // Edit operations
        Undo = 2000,
        Redo,
        Cut,
        Copy,
        Paste,
        Delete,
        SelectAll,

        // Playback controls
        Play = 3000,
        Pause,
        Stop,
        Record,
        Loop,

        // View operations
        ZoomIn = 4000,
        ZoomOut,
        ZoomToFit,
        ToggleFullscreen,

        // Audio operations
        Normalize = 5000,
        FadeIn,
        FadeOut,
        Reverse,

        // Application
        Preferences = 6000,
        ShowHelp,
        About
    };

    //==========================================================================
    // Command info structure
    struct Command
    {
        CommandID id;
        juce::String name;
        juce::String category;
        juce::String description;
        juce::KeyPress defaultKeyPress;
        std::function<void()> action;
        std::function<bool()> isEnabled = []() { return true; };
    };

    //==========================================================================
    // Singleton access
    static KeyboardShortcuts& getInstance()
    {
        static KeyboardShortcuts instance;
        return instance;
    }

    //==========================================================================
    // Command registration
    void registerCommand(const Command& cmd)
    {
        commands[cmd.id] = cmd;

        // Set default key binding if not already set
        if (!keyBindings.count(cmd.defaultKeyPress))
        {
            keyBindings[cmd.defaultKeyPress] = cmd.id;
        }
    }

    void unregisterCommand(CommandID id)
    {
        // Remove from commands
        commands.erase(id);

        // Remove key bindings
        for (auto it = keyBindings.begin(); it != keyBindings.end();)
        {
            if (it->second == id)
                it = keyBindings.erase(it);
            else
                ++it;
        }
    }

    //==========================================================================
    // Key binding management
    bool setKeyBinding(CommandID id, const juce::KeyPress& key)
    {
        // Check if key is already bound
        if (keyBindings.count(key) && keyBindings[key] != id)
        {
            // Key conflict - remove old binding
            keyBindings.erase(key);
        }

        // Remove any existing binding for this command
        for (auto it = keyBindings.begin(); it != keyBindings.end();)
        {
            if (it->second == id)
                it = keyBindings.erase(it);
            else
                ++it;
        }

        // Set new binding
        keyBindings[key] = id;
        return true;
    }

    juce::KeyPress getKeyBinding(CommandID id) const
    {
        for (const auto& pair : keyBindings)
        {
            if (pair.second == id)
                return pair.first;
        }
        return juce::KeyPress();
    }

    void clearKeyBinding(CommandID id)
    {
        for (auto it = keyBindings.begin(); it != keyBindings.end();)
        {
            if (it->second == id)
                it = keyBindings.erase(it);
            else
                ++it;
        }
    }

    //==========================================================================
    // Command execution
    bool handleKeyPress(const juce::KeyPress& key)
    {
        auto it = keyBindings.find(key);
        if (it == keyBindings.end())
            return false;

        CommandID cmdId = it->second;
        return executeCommand(cmdId);
    }

    bool executeCommand(CommandID id)
    {
        auto it = commands.find(id);
        if (it == commands.end())
            return false;

        const Command& cmd = it->second;

        // Check if command is enabled
        if (!cmd.isEnabled())
            return false;

        // Execute action
        if (cmd.action)
        {
            cmd.action();
            return true;
        }

        return false;
    }

    //==========================================================================
    // Query methods
    bool hasCommand(CommandID id) const
    {
        return commands.count(id) > 0;
    }

    const Command* getCommand(CommandID id) const
    {
        auto it = commands.find(id);
        return it != commands.end() ? &it->second : nullptr;
    }

    juce::StringArray getCommandCategories() const
    {
        juce::StringArray categories;
        for (const auto& pair : commands)
        {
            if (!categories.contains(pair.second.category))
                categories.add(pair.second.category);
        }
        categories.sort(true);
        return categories;
    }

    std::vector<CommandID> getCommandsInCategory(const juce::String& category) const
    {
        std::vector<CommandID> result;
        for (const auto& pair : commands)
        {
            if (pair.second.category == category)
                result.push_back(pair.first);
        }
        return result;
    }

    //==========================================================================
    // Persistence
    void saveBindings(juce::PropertiesFile* props)
    {
        if (!props)
            return;

        juce::XmlElement root("KeyboardShortcuts");

        for (const auto& pair : keyBindings)
        {
            auto* binding = root.createNewChildElement("Binding");
            binding->setAttribute("command", static_cast<int>(pair.second));
            binding->setAttribute("keyCode", pair.first.getKeyCode());
            binding->setAttribute("modifiers", pair.first.getModifiers().getRawFlags());
        }

        props->setValue("KeyboardShortcuts", &root);
    }

    void loadBindings(juce::PropertiesFile* props)
    {
        if (!props)
            return;

        juce::XmlElement* root = props->getXmlValue("KeyboardShortcuts");
        if (!root)
            return;

        keyBindings.clear();

        for (auto* binding : root->getChildIterator())
        {
            if (binding->hasTagName("Binding"))
            {
                CommandID id = static_cast<CommandID>(binding->getIntAttribute("command"));
                int keyCode = binding->getIntAttribute("keyCode");
                int modifiers = binding->getIntAttribute("modifiers");

                juce::KeyPress key(keyCode, juce::ModifierKeys(modifiers), 0);
                keyBindings[key] = id;
            }
        }

        delete root;
    }

    void resetToDefaults()
    {
        keyBindings.clear();

        for (const auto& pair : commands)
        {
            if (pair.second.defaultKeyPress.isValid())
                keyBindings[pair.second.defaultKeyPress] = pair.first;
        }
    }

private:
    KeyboardShortcuts() = default;
    ~KeyboardShortcuts() = default;

    // Non-copyable
    KeyboardShortcuts(const KeyboardShortcuts&) = delete;
    KeyboardShortcuts& operator=(const KeyboardShortcuts&) = delete;

    std::map<CommandID, Command> commands;
    std::map<juce::KeyPress, CommandID> keyBindings;
};

//==============================================================================
/**
    Component that handles keyboard shortcuts.
    Add as a KeyListener to components that should respond to shortcuts.
*/
class ShortcutHandler : public juce::KeyListener
{
public:
    ShortcutHandler() = default;

    bool keyPressed(const juce::KeyPress& key, juce::Component*) override
    {
        return KeyboardShortcuts::getInstance().handleKeyPress(key);
    }
};

//==============================================================================
/**
    Helper to register common application shortcuts.
*/
class StandardShortcuts
{
public:
    static void registerAll()
    {
        auto& shortcuts = KeyboardShortcuts::getInstance();

        using CommandID = KeyboardShortcuts::CommandID;
        using Command = KeyboardShortcuts::Command;

        // File operations
        shortcuts.registerCommand({
            CommandID::NewProject,
            "New Project",
            "File",
            "Create a new project",
            juce::KeyPress('n', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::OpenProject,
            "Open Project",
            "File",
            "Open an existing project",
            juce::KeyPress('o', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::SaveProject,
            "Save Project",
            "File",
            "Save the current project",
            juce::KeyPress('s', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::SaveProjectAs,
            "Save Project As",
            "File",
            "Save the current project with a new name",
            juce::KeyPress('s', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier, 0),
            nullptr
        });

        // Edit operations
        shortcuts.registerCommand({
            CommandID::Undo,
            "Undo",
            "Edit",
            "Undo the last action",
            juce::KeyPress('z', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::Redo,
            "Redo",
            "Edit",
            "Redo the last undone action",
            juce::KeyPress('z', juce::ModifierKeys::commandModifier | juce::ModifierKeys::shiftModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::Cut,
            "Cut",
            "Edit",
            "Cut selection to clipboard",
            juce::KeyPress('x', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::Copy,
            "Copy",
            "Edit",
            "Copy selection to clipboard",
            juce::KeyPress('c', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::Paste,
            "Paste",
            "Edit",
            "Paste from clipboard",
            juce::KeyPress('v', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::SelectAll,
            "Select All",
            "Edit",
            "Select all content",
            juce::KeyPress('a', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        // Playback controls
        shortcuts.registerCommand({
            CommandID::Play,
            "Play",
            "Playback",
            "Start playback",
            juce::KeyPress(juce::KeyPress::spaceKey, juce::ModifierKeys::noModifiers, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::Stop,
            "Stop",
            "Playback",
            "Stop playback",
            juce::KeyPress(juce::KeyPress::escapeKey, juce::ModifierKeys::noModifiers, 0),
            nullptr
        });

        // View operations
        shortcuts.registerCommand({
            CommandID::ZoomIn,
            "Zoom In",
            "View",
            "Zoom in the view",
            juce::KeyPress('=', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::ZoomOut,
            "Zoom Out",
            "View",
            "Zoom out the view",
            juce::KeyPress('-', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });

        shortcuts.registerCommand({
            CommandID::ZoomToFit,
            "Zoom to Fit",
            "View",
            "Fit content to window",
            juce::KeyPress('0', juce::ModifierKeys::commandModifier, 0),
            nullptr
        });
    }
};

} // namespace UI
} // namespace TSS
