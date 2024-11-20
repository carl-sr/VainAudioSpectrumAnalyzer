#pragma once

//--------------------------------------------------------------------------------

#include <juce_dsp/juce_dsp.h>
#include "AudioBufferFifo.h"
#include "SpectrumAnalyzerBuffer.h"
#include "SpectrumAnalyzerCurve.h"

//--------------------------------------------------------------------------------
/**
 * @class SpectrumAnalyzer
 * @brief runs an fft in a time slice thread over whatever data it gets
 *
*/
//--------------------------------------------------------------------------------

class SpectrumAnalyzer : public juce::TimeSliceClient
{
public:
    explicit SpectrumAnalyzer(AudioBufferFifo &source);
    ~SpectrumAnalyzer() override;

    SpectrumAnalyzerCurve::LockedCurve getAnalyzerCurve();
    
    // juce::TimeSliceClient
    int useTimeSlice() override;
    static void stopTimeSliceThread();

private:
    AudioBufferFifo &m_audioFifo;

    std::array<SpectrumAnalyzerBuffer, 5> m_buffers;
    std::vector<float> m_conduitBuffer;

    juce::dsp::FFT m_fft{ 11 };
    juce::dsp::WindowingFunction<float> m_windowingFunction{ static_cast<std::size_t>(m_fft.getSize()), juce::dsp::WindowingFunction<float>::hann, true };

    SpectrumAnalyzerCurve m_analyzerCurve{ m_fft.getSize() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};

//--------------------------------------------------------------------------------
