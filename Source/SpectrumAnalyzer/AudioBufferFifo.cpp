#include "AudioBufferFifo.h"

//--------------------------------------------------------------------------------

AudioBufferFifo::AudioBufferFifo() = default;

//--------------------------------------------------------------------------------

void AudioBufferFifo::setSize(double sampleRate, double lengthInSeconds)
{
    m_sampleRate = sampleRate;
    const int length{ static_cast<int>(sampleRate * lengthInSeconds) };
    std::lock_guard<std::mutex> guard{ m_fifoResizeLock };

    m_abstractFifo.setTotalSize(length);
    m_audioFifo.clear();
    m_audioFifo.insert(m_audioFifo.begin(), static_cast<std::size_t>(length), 0.0f);
}

//--------------------------------------------------------------------------------

void AudioBufferFifo::pushAudioData(const juce::AudioBuffer<float> &buffer)
{
    jassert(!m_audioFifo.empty()); // call setSize

    const juce::AbstractFifo::ScopedWrite write{ m_abstractFifo, buffer.getNumSamples() };

    if(write.blockSize1 > 0)
    {
        const float * const dataRead{ buffer.getReadPointer(0) };
        const std::span<const float> inSpan{ dataRead, dataRead + write.blockSize1 };

        const auto outBegin{ m_audioFifo.begin() + write.startIndex1 };
        std::ranges::copy(inSpan, outBegin);
    }

    if(write.blockSize2 > 0)
    {
        const float * const dataRead{ buffer.getReadPointer(0) + write.blockSize1 };
        const std::span<const float> inSpan{ dataRead, dataRead + write.blockSize2 };

        const auto outBegin{ m_audioFifo.begin() + write.startIndex2 };
        std::ranges::copy(inSpan, outBegin);
    }
}

//--------------------------------------------------------------------------------

void AudioBufferFifo::pushSample(const float &sample)
{
    juce::AbstractFifo::ScopedWrite write{ m_abstractFifo, 1 };
    if(write.blockSize1 > 0)
        m_audioFifo[static_cast<std::size_t>(write.startIndex1)] = sample;

    jassert(write.blockSize2 == 0);
}

//--------------------------------------------------------------------------------

void AudioBufferFifo::popAudioData(std::span<float> dest)
{
    std::lock_guard<std::mutex> guard{ m_fifoResizeLock };

    const juce::AbstractFifo::ScopedRead read{ m_abstractFifo, static_cast<int>(dest.size()) };

    if(read.blockSize1 > 0)
    {
        std::span<float> readSpan{ m_audioFifo.begin() + read.startIndex1,
                                   m_audioFifo.begin() + read.startIndex1 + read.blockSize1 };

        std::ranges::copy(readSpan, dest.begin());
    }

    if(read.blockSize2 > 0)
    {
        std::span<float> readSpan{ m_audioFifo.begin() + read.startIndex2,
                                   m_audioFifo.begin() + read.startIndex2 + read.blockSize2 };

        std::ranges::copy(readSpan, dest.begin() + read.blockSize1);
    }
}

//--------------------------------------------------------------------------------

int AudioBufferFifo::getAvailableSamples() const
{
    return m_abstractFifo.getNumReady();
}

//--------------------------------------------------------------------------------

double AudioBufferFifo::getSampleRate() const
{
    return m_sampleRate;
}

//--------------------------------------------------------------------------------
