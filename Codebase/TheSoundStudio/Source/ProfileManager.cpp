/*
  ==============================================================================

    ProfileManager.cpp
    Created: 12 Sep 2025
    Author:  The Sound Studio Team
    
    Simple implementation for save/load profile functionality

  ==============================================================================
*/

#include "ProfileManager.h"

ProfileManager::ProfileManager(ProjectManager* pm) : projectManager(pm)
{
    // Create profile directory if it doesn't exist
    profilesDirectory = File::getSpecialLocation(File::userDocumentsDirectory)
                           .getChildFile("TSS")
                           .getChildFile("Profiles");
    
    if (!profilesDirectory.exists())
        profilesDirectory.createDirectory();
}

ProfileManager::~ProfileManager()
{
}

bool ProfileManager::saveProfile(ProfileType type, int shortcutIndex, const String& profileName)
{
    if (profileName.isEmpty())
        return false;
    
    // Create subdirectory for profile type
    File typeDir = profilesDirectory.getChildFile(getProfileTypeName(type));
    if (!typeDir.exists())
        typeDir.createDirectory();
    
    // Create profile file
    File profileFile = typeDir.getChildFile(profileName + ".xml");
    
    // Create XML structure
    std::unique_ptr<XmlElement> xml(new XmlElement("Profile"));
    xml->setAttribute("type", getProfileTypeName(type));
    xml->setAttribute("version", "1.0");
    xml->setAttribute("timestamp", Time::getCurrentTime().toISO8601(true));
    
    // Add parameters based on type
    XmlElement* params = xml->createNewChildElement("Parameters");
    
    switch (type)
    {
        case CHORD_PLAYER:
            saveChordPlayerProfile(params, shortcutIndex);
            break;
            
        case FREQUENCY_PLAYER:
            saveFrequencyPlayerProfile(params, shortcutIndex);
            break;
            
        case CHORD_SCANNER:
            saveChordScannerProfile(params);
            break;
            
        case FREQUENCY_SCANNER:
            saveFrequencyScannerProfile(params);
            break;
            
        case REALTIME_ANALYSIS:
            saveRealtimeAnalysisProfile(params);
            break;
            
        case LISSAJOUS:
            saveLissajousProfile(params);
            break;
    }
    
    // Write to file
    return xml->writeTo(profileFile);
}

bool ProfileManager::loadProfile(ProfileType type, int shortcutIndex, const String& profileName)
{
    File typeDir = profilesDirectory.getChildFile(getProfileTypeName(type));
    File profileFile = typeDir.getChildFile(profileName + ".xml");
    
    if (!profileFile.exists())
        return false;
    
    std::unique_ptr<XmlElement> xml(XmlDocument::parse(profileFile));
    if (!xml || xml->getTagName() != "Profile")
        return false;
    
    XmlElement* params = xml->getChildByName("Parameters");
    if (!params)
        return false;
    
    switch (type)
    {
        case CHORD_PLAYER:
            loadChordPlayerProfile(params, shortcutIndex);
            break;
            
        case FREQUENCY_PLAYER:
            loadFrequencyPlayerProfile(params, shortcutIndex);
            break;
            
        case CHORD_SCANNER:
            loadChordScannerProfile(params);
            break;
            
        case FREQUENCY_SCANNER:
            loadFrequencyScannerProfile(params);
            break;
            
        case REALTIME_ANALYSIS:
            loadRealtimeAnalysisProfile(params);
            break;
            
        case LISSAJOUS:
            loadLissajousProfile(params);
            break;
    }
    
    return true;
}

StringArray ProfileManager::getAvailableProfiles(ProfileType type)
{
    StringArray profiles;
    File typeDir = profilesDirectory.getChildFile(getProfileTypeName(type));
    
    if (typeDir.exists())
    {
        Array<File> files = typeDir.findChildFiles(File::findFiles, false, "*.xml");
        for (auto& file : files)
        {
            profiles.add(file.getFileNameWithoutExtension());
        }
    }
    
    return profiles;
}

bool ProfileManager::deleteProfile(ProfileType type, const String& profileName)
{
    File typeDir = profilesDirectory.getChildFile(getProfileTypeName(type));
    File profileFile = typeDir.getChildFile(profileName + ".xml");
    
    if (profileFile.exists())
        return profileFile.deleteFile();
    
    return false;
}

void ProfileManager::saveChordPlayerProfile(XmlElement* params, int shortcutIndex)
{
    // For now, just save basic parameters
    // TODO: Complete implementation based on actual parameter mapping
    params->setAttribute("shortcut", shortcutIndex);
    params->setAttribute("saved", true);
}

void ProfileManager::loadChordPlayerProfile(XmlElement* params, int shortcutIndex)
{
    // TODO: Complete implementation based on actual parameter mapping
}

void ProfileManager::saveFrequencyPlayerProfile(XmlElement* params, int shortcutIndex)
{
    // TODO: Complete implementation
    params->setAttribute("shortcut", shortcutIndex);
}

void ProfileManager::loadFrequencyPlayerProfile(XmlElement* params, int shortcutIndex)
{
    // TODO: Complete implementation
}

void ProfileManager::saveChordScannerProfile(XmlElement* params)
{
    // Save chord scanner parameters
    params->setAttribute("amplitude", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_AMPLITUDE));
    params->setAttribute("attack", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ATTACK));
    params->setAttribute("decay", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_DECAY));
    params->setAttribute("sustain", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_SUSTAIN));
    params->setAttribute("release", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_RELEASE));
}

void ProfileManager::loadChordScannerProfile(XmlElement* params)
{
    projectManager->setChordScannerParameter(CHORD_SCANNER_AMPLITUDE, params->getDoubleAttribute("amplitude"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_ATTACK, params->getDoubleAttribute("attack"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_DECAY, params->getDoubleAttribute("decay"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_SUSTAIN, params->getDoubleAttribute("sustain"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_RELEASE, params->getDoubleAttribute("release"));
}

void ProfileManager::saveFrequencyScannerProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

void ProfileManager::loadFrequencyScannerProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

void ProfileManager::saveRealtimeAnalysisProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

void ProfileManager::loadRealtimeAnalysisProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

void ProfileManager::saveLissajousProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

void ProfileManager::loadLissajousProfile(XmlElement* params)
{
    // TODO: Complete implementation
}

String ProfileManager::getProfileTypeName(ProfileType type)
{
    switch (type)
    {
        case CHORD_PLAYER:      return "ChordPlayer";
        case FREQUENCY_PLAYER:  return "FrequencyPlayer";
        case CHORD_SCANNER:     return "ChordScanner";
        case FREQUENCY_SCANNER: return "FrequencyScanner";
        case REALTIME_ANALYSIS: return "RealtimeAnalysis";
        case LISSAJOUS:         return "Lissajous";
        default:                return "Unknown";
    }
}