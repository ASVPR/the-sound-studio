/*
  ==============================================================================

    ProfileManager.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Parameters.h"

class ProjectManager;

class ProfileManager
{
public:
    ProfileManager(ProjectManager* pm);
    ~ProfileManager();
    
    // Profile types for different modules
    enum ProfileType
    {
        CHORD_PLAYER_PROFILE,
        CHORD_SCANNER_PROFILE,
        FREQUENCY_PLAYER_PROFILE,
        FREQUENCY_SCANNER_PROFILE
    };
    
    // Save profile for a specific module and shortcut
    bool saveProfile(ProfileType type, int shortcutIndex, const String& profileName = "");
    
    // Load profile for a specific module and shortcut
    bool loadProfile(ProfileType type, int shortcutIndex);
    
    // Get list of available profiles for a module type
    StringArray getAvailableProfiles(ProfileType type);
    
    // Delete a profile
    bool deleteProfile(ProfileType type, const String& profileName);
    
    // Get default profile directory
    File getProfilesDirectory();
    
    // Create default profile name
    String createDefaultProfileName(ProfileType type);
    
private:
    ProjectManager* projectManager;
    
    // Get subdirectory for specific profile type
    File getProfileTypeDirectory(ProfileType type);
    
    // Get profile type as string
    String getProfileTypeName(ProfileType type);
    
    // Save/Load helpers
    void saveChordPlayerProfile(int shortcutIndex, XmlElement* xml);
    void loadChordPlayerProfile(int shortcutIndex, XmlElement* xml);
    
    void saveChordScannerProfile(XmlElement* xml);
    void loadChordScannerProfile(XmlElement* xml);
    
    void saveFrequencyPlayerProfile(int shortcutIndex, XmlElement* xml);
    void loadFrequencyPlayerProfile(int shortcutIndex, XmlElement* xml);
    
    void saveFrequencyScannerProfile(XmlElement* xml);
    void loadFrequencyScannerProfile(XmlElement* xml);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProfileManager)
};