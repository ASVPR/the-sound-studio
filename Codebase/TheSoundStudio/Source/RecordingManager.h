/*
  ==============================================================================

    RecordingManager.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TSSPaths.h"
#include "TSSConstants.h"
#include <memory>

class RecordingManager
{
public:
    RecordingManager()
        : backgroundThread("Audio Recorder Thread")
    {
        backgroundThread.startThread();
    }

    ~RecordingManager()
    {
        stopRecording();
        if (backgroundThread.isThreadRunning())
            backgroundThread.stopThread(1000);
    }

    void setupRecording(const juce::File& file,
                        juce::AudioFormatManager& formatManager,
                        int formatIndex,
                        double sampleRate)
    {
        stopRecording();

        file.deleteFile();

        auto fileStream = std::make_unique<juce::FileOutputStream>(file);

        if (fileStream != nullptr && fileStream->openedOk())
        {
            auto* format = formatManager.getKnownFormat(formatIndex);
            if (format != nullptr)
            {
                auto* writer = format->createWriterFor(
                    fileStream.get(), sampleRate, 2, 24, {}, 0);

                if (writer != nullptr)
                {
                    fileStream.release();

                    threadedWriter = std::make_unique<juce::AudioFormatWriter::ThreadedWriter>(
                        writer, backgroundThread, TSS::Audio::kRecordingBufferSize);

                    const juce::ScopedLock sl(writerLock);
                    activeWriter = threadedWriter.get();
                    recordCounterInSamples = 0;
                }
            }
        }
    }

    void createAndStartRecording(const juce::String& prefix,
                                  const juce::File& recordDirectory,
                                  juce::AudioFormatManager& formatManager,
                                  int formatIndex,
                                  double sampleRate)
    {
        auto filename = TSS::TSSPaths::buildTimestampedFilename(prefix, ".wav");
        juce::File newFile(recordDirectory.getChildFile(filename));
        newFile.create();
        setupRecording(newFile, formatManager, formatIndex, sampleRate);
    }

    void startRecording()
    {
        shouldRecord = true;
    }

    void stopRecording()
    {
        {
            const juce::ScopedLock sl(writerLock);
            activeWriter = nullptr;
        }

        threadedWriter.reset();
        shouldRecord = false;
    }

    bool isRecording() const
    {
        return activeWriter != nullptr;
    }

    bool getShouldRecord() const { return shouldRecord; }

    void writeBlock(const float* const* data, int numSamples)
    {
        const juce::ScopedLock sl(writerLock);
        if (activeWriter != nullptr)
        {
            activeWriter->write(data, numSamples);
            recordCounterInSamples += static_cast<uint64>(numSamples);
        }
    }

    uint64 getRecordCounterInSamples() const { return recordCounterInSamples; }

    uint64 getRecordCounterInMilliseconds(double sampleRate) const
    {
        if (sampleRate <= 0.0) return 0;
        return static_cast<uint64>((recordCounterInSamples / sampleRate) * 1000.0);
    }

private:
    juce::TimeSliceThread backgroundThread;
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> threadedWriter;

    juce::CriticalSection writerLock;
    juce::AudioFormatWriter::ThreadedWriter* activeWriter = nullptr;

    bool shouldRecord = false;
    uint64 recordCounterInSamples = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordingManager)
};
