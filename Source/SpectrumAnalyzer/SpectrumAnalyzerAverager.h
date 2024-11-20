#pragma once

//-------------------------------------------------------------------------------------------------------
/**
 * @class SpectrumAnalyzerAverager
 * @brief calculate the average over the last n buffers (n = 5 currently)
 *
 * getReadSpan() runs the average
 *
*/
//-------------------------------------------------------------------------------------------------------

#include <juce_dsp/juce_dsp.h>
#include <span>

//--------------------------------------------------------------------------------

class SpectrumAnalyzerAverager
{
public:
    explicit SpectrumAnalyzerAverager(int fftSize);

    void pushCurve(std::span<const float> fftCurve);

    std::span<const float> getReadSpan();

private:
    juce::AudioBuffer<float> m_outputBuffer;
    juce::AudioBuffer<float> m_averagerBuffer;
    int m_averagerChannelIndex{ 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzerAverager)
};

//--------------------------------------------------------------------------------
