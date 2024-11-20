#include "SpectrumAnalyzerAverager.h"

//--------------------------------------------------------------------------------

SpectrumAnalyzerAverager::SpectrumAnalyzerAverager(int fftSize)
    : m_outputBuffer{ 1, fftSize / 2 }
    , m_averagerBuffer{ 5, fftSize / 2 }
{
    m_averagerBuffer.clear();
    m_outputBuffer.clear();
}

//--------------------------------------------------------------------------------

void SpectrumAnalyzerAverager::pushCurve(std::span<const float> fftCurve)
{
    const float gain{ 1.0f / static_cast<float>(m_averagerBuffer.getNumSamples() * m_averagerBuffer.getNumChannels()) };
    m_averagerBuffer.copyFrom(m_averagerChannelIndex, 0, &fftCurve.front(), m_averagerBuffer.getNumSamples(), gain);

    if (++m_averagerChannelIndex == m_averagerBuffer.getNumChannels())
        m_averagerChannelIndex = 0;
}

//--------------------------------------------------------------------------------

std::span<const float> SpectrumAnalyzerAverager::getReadSpan()
{
    m_outputBuffer.clear();

    for(int c = 0; c < m_averagerBuffer.getNumChannels(); ++c)
    {
        m_outputBuffer.addFrom(0, 0, m_averagerBuffer, c, 0, m_averagerBuffer.getNumSamples(), 2.0f);
    }

    juce::FloatVectorOperations::multiply(m_outputBuffer.getWritePointer(0), 1.0f / static_cast<float>(m_averagerBuffer.getNumChannels()), m_outputBuffer.getNumSamples());

    return { m_outputBuffer.getReadPointer(0), static_cast<std::size_t>(m_outputBuffer.getNumSamples()) };
}

//--------------------------------------------------------------------------------
