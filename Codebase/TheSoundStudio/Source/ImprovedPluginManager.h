/*
  ==============================================================================

    ImprovedPluginManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class ImprovedPluginManager : public Component,
                              public ChangeListener,
                              private Timer
{
public:
    ImprovedPluginManager();
    ~ImprovedPluginManager() override;
    
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    
    // Plugin management
    void initializeFormats();
    void scanForPlugins();
    bool loadPlugin(const PluginDescription& description);
    void unloadCurrentPlugin();
    
    // Audio processing
    void prepareToPlay(double sampleRate, int blockSize);
    void processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiBuffer);
    void releaseResources();
    
    // Plugin window management
    void showPluginWindow(bool shouldShow = true);
    bool hasPluginWindow() const { return pluginWindow != nullptr; }
    
    // State management
    void getStateInformation(MemoryBlock& destData);
    void setStateInformation(const void* data, int sizeInBytes);
    
    // Plugin information
    String getCurrentPluginName() const;
    bool hasPluginLoaded() const { return currentPlugin != nullptr; }
    
    // Callbacks
    std::function<void(bool)> onPluginLoadedCallback;
    
private:
    // Plugin format management
    AudioPluginFormatManager formatManager;
    KnownPluginList knownPluginList;
    std::unique_ptr<PluginListComponent> pluginListComponent;
    
    // Current plugin
    std::unique_ptr<AudioPluginInstance> currentPlugin;
    std::unique_ptr<AudioProcessorEditor> pluginEditor;
    std::unique_ptr<DocumentWindow> pluginWindow;
    
    // Audio processing
    double sampleRate = 44100.0;
    int blockSize = 512;
    
    // UI Components
    TextButton loadButton, unloadButton, showUIButton;
    Label statusLabel;
    ComboBox pluginSelector;
    
    // Plugin scanning
    std::unique_ptr<PluginDirectoryScanner> scanner;
    ThreadPool threadPool;
    
    // ChangeListener override
    void changeListenerCallback(ChangeBroadcaster* source) override;
    
    // Timer override  
    void timerCallback() override;
    
    // Internal methods
    void updatePluginSelector();
    void updateStatusLabel();
    void createPluginWindow();
    void closePluginWindow();
    
    // Button callbacks
    void loadButtonClicked();
    void unloadButtonClicked();  
    void showUIButtonClicked();
    void pluginSelectorChanged();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImprovedPluginManager)
};