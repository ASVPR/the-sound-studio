/*
  ==============================================================================

    ProfileManager.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "ProfileManager.h"
#include "ProjectManager.h"

ProfileManager::ProfileManager(ProjectManager* pm) : projectManager(pm)
{
}

ProfileManager::~ProfileManager()
{
}

bool ProfileManager::saveProfile(ProfileType type, int shortcutIndex, const String& profileName)
{
    File profilesDir = getProfilesDirectory();
    if (!profilesDir.exists())
        profilesDir.createDirectory();
    
    File typeDir = getProfileTypeDirectory(type);
    if (!typeDir.exists())
        typeDir.createDirectory();
    
    String fileName = profileName.isEmpty() ? createDefaultProfileName(type) : profileName;
    File profileFile = typeDir.getChildFile(fileName + ".xml");
    
    std::unique_ptr<XmlElement> xml(new XmlElement("Profile"));
    xml->setAttribute("type", getProfileTypeName(type));
    xml->setAttribute("version", "1.0");
    
    XmlElement* params = xml->createNewChildElement("Parameters");
    
    switch (type)
    {
        case CHORD_PLAYER_PROFILE:
            saveChordPlayerProfile(shortcutIndex, params);
            break;
        case CHORD_SCANNER_PROFILE:
            saveChordScannerProfile(params);
            break;
        case FREQUENCY_PLAYER_PROFILE:
            saveFrequencyPlayerProfile(shortcutIndex, params);
            break;
        case FREQUENCY_SCANNER_PROFILE:
            saveFrequencyScannerProfile(params);
            break;
    }
    
    return xml->writeTo(profileFile);
}

bool ProfileManager::loadProfile(ProfileType type, int shortcutIndex)
{
    // TODO: Implement file chooser and loading
    return false;
}

StringArray ProfileManager::getAvailableProfiles(ProfileType type)
{
    StringArray profiles;
    File typeDir = getProfileTypeDirectory(type);
    
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
    File typeDir = getProfileTypeDirectory(type);
    File profileFile = typeDir.getChildFile(profileName + ".xml");
    
    if (profileFile.exists())
        return profileFile.deleteFile();
    
    return false;
}

File ProfileManager::getProfilesDirectory()
{
    return File::getSpecialLocation(File::userDocumentsDirectory)
               .getChildFile("TSS")
               .getChildFile("Profiles");
}

String ProfileManager::createDefaultProfileName(ProfileType type)
{
    return getProfileTypeName(type) + "_" + Time::getCurrentTime().formatted("%Y%m%d_%H%M%S");
}

File ProfileManager::getProfileTypeDirectory(ProfileType type)
{
    return getProfilesDirectory().getChildFile(getProfileTypeName(type));
}

String ProfileManager::getProfileTypeName(ProfileType type)
{
    switch (type)
    {
        case CHORD_PLAYER_PROFILE:      return "ChordPlayer";
        case CHORD_SCANNER_PROFILE:     return "ChordScanner";
        case FREQUENCY_PLAYER_PROFILE:  return "FrequencyPlayer";
        case FREQUENCY_SCANNER_PROFILE: return "FrequencyScanner";
        default:                        return "Unknown";
    }
}

void ProfileManager::saveChordPlayerProfile(int shortcutIndex, XmlElement* xml)
{
    // TODO: Save actual chord player parameters
    xml->setAttribute("shortcut", shortcutIndex);
}

void ProfileManager::loadChordPlayerProfile(int shortcutIndex, XmlElement* xml)
{
    // TODO: Load actual chord player parameters
}

void ProfileManager::saveChordScannerProfile(XmlElement* xml)
{
    // Save chord scanner ADSR parameters
    xml->setAttribute("amplitude", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE));
    xml->setAttribute("attack", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_ATTACK));
    xml->setAttribute("decay", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_DECAY));
    xml->setAttribute("sustain", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN));
    xml->setAttribute("release", (double)projectManager->getChordScannerParameter(CHORD_SCANNER_ENV_RELEASE));
}

void ProfileManager::loadChordScannerProfile(XmlElement* xml)
{
    projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_AMPLITUDE, xml->getDoubleAttribute("amplitude"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_ATTACK, xml->getDoubleAttribute("attack"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_DECAY, xml->getDoubleAttribute("decay"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_SUSTAIN, xml->getDoubleAttribute("sustain"));
    projectManager->setChordScannerParameter(CHORD_SCANNER_ENV_RELEASE, xml->getDoubleAttribute("release"));
}

void ProfileManager::saveFrequencyPlayerProfile(int shortcutIndex, XmlElement* xml)
{
    // TODO: Save frequency player parameters
    xml->setAttribute("shortcut", shortcutIndex);
}

void ProfileManager::loadFrequencyPlayerProfile(int shortcutIndex, XmlElement* xml)
{
    // TODO: Load frequency player parameters
}

void ProfileManager::saveFrequencyScannerProfile(XmlElement* xml)
{
    // TODO: Save frequency scanner parameters
}

void ProfileManager::loadFrequencyScannerProfile(XmlElement* xml)
{
    // TODO: Load frequency scanner parameters
}