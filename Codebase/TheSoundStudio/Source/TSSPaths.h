/*
  ==============================================================================

    TSSPaths.h

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TSSConstants.h"

namespace TSS {

class TSSPaths
{
public:
    static juce::File getAppDataDirectory()
    {
        return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile(TSS::Paths::kAppDataDir);
    }

    static juce::File getLogsDirectory()
    {
        auto dir = getAppDataDirectory().getChildFile(TSS::Paths::kLogsSubDir);
        dir.createDirectory();
        return dir;
    }

    static juce::File getRecordingsDirectory()
    {
        auto dir = getAppDataDirectory().getChildFile(TSS::Paths::kRecordingsDir);
        dir.createDirectory();
        return dir;
    }

    static juce::File getProfilesDirectory()
    {
        auto dir = getAppDataDirectory().getChildFile(TSS::Paths::kProfilesDir);
        dir.createDirectory();
        return dir;
    }

    static juce::String buildTimestampedFilename(const juce::String& prefix,
                                                  const juce::String& extension = TSS::Paths::kLogExtension)
    {
        auto time = juce::Time::getCurrentTime();

        return juce::String::formatted("%02d.%02d.%d-%02d-%02d-%s%s",
            time.getDayOfMonth(),
            time.getMonth() + 1,
            time.getYear(),
            time.getHours(),
            time.getMinutes(),
            prefix.toRawUTF8(),
            extension.toRawUTF8());
    }

private:
    TSSPaths() = delete;
};

} // namespace TSS
