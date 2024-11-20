#pragma once

//-------------------------------------------------------------------------------------------------------
/**
 * @class AudioBufferFifo
 * @brief This is the part that lives in the processor and accepts new audio data
 *
 * audio thread only: setSize(), pushAudioData()
 * ui thread only: popAudioData()
 *
 * a lock ensures the vector cannot be resized while audio is being read.
 * it does note exist while audio is being written - writing and resizing should
 * happen only on the same thread.
 *
*/
//-------------------------------------------------------------------------------------------------------

#include <juce_dsp/juce_dsp.h>
#include <span>

//--------------------------------------------------------------------------------

class AudioBufferFifo
{
public:
    AudioBufferFifo();

    void setSize(double sampleRate, double lengthInSeconds);
    void pushAudioData(const juce::AudioBuffer<float> &buffer);
    void pushSample(const float &sample);
    void popAudioData(std::span<float> dest);

    int getAvailableSamples() const;
    double getSampleRate() const;

private:
    double m_sampleRate{};
    std::mutex m_fifoResizeLock;
    juce::AbstractFifo m_abstractFifo{ 1 };
    std::vector<float> m_audioFifo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioBufferFifo)
};

//--------------------------------------------------------------------------------
