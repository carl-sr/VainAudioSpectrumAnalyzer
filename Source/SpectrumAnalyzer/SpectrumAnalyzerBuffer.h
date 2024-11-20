#pragma once

//--------------------------------------------------------------------------------
/**
 * @class SpectrumAnalyzerBuffer
 * @brief double buffer for reading/writing
 *
 * write into a buffer, perform fft once the buffer is full lives in the editor
 * and receives new audio data from AudioBufferFifo
 *
 * @see AudioBufferFifo
 *
*/
//--------------------------------------------------------------------------------

#include <juce_dsp/juce_dsp.h>
#include <span>

//--------------------------------------------------------------------------------

class SpectrumAnalyzerBuffer
{
public:
    SpectrumAnalyzerBuffer();

    void setSize(const std::size_t& size);
    int getSize() const;

    using FftSpan = std::span<float>;
    FftSpan getFftSpan();

    void write(std::span<const float> data);
    void skip(int samples);

    bool isFftSpanReady() const;

private:
    std::size_t getWriteIndex() const;
    void addToWriteIndex(int n);

    std::size_t m_writeIndex{ 0 };
    std::vector<float> m_writeBuffer;
    std::vector<float> m_fftBuffer;
    bool m_isFftBufferReady{ false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzerBuffer)
};

//--------------------------------------------------------------------------------
