#include "SpectrumAnalyzer.h"

//--------------------------------------------------------------------------------

class SpectrumAnalyzerTimeSliceThread : public juce::TimeSliceThread
{
public:
    SpectrumAnalyzerTimeSliceThread() : juce::TimeSliceThread("fft") { startThread(); }
    ~SpectrumAnalyzerTimeSliceThread() override { stopThread(100); }
};
static SpectrumAnalyzerTimeSliceThread TimeSliceThread;

//--------------------------------------------------------------------------------

SpectrumAnalyzer::SpectrumAnalyzer(AudioBufferFifo &source)
    : m_audioFifo(source)
{
    const int fftSize = m_fft.getSize() * 2;
    const int stepSize = fftSize / static_cast<int>(m_buffers.size());
    for(const auto &[index, buffer] : juce::enumerate(m_buffers))
    {
        buffer.setSize(static_cast<std::size_t>(fftSize));
        const int writeSize = stepSize * static_cast<int>(index);
        if (writeSize != 0)
            buffer.skip(writeSize);
    }

    TimeSliceThread.addTimeSliceClient(this);
}

//--------------------------------------------------------------------------------

SpectrumAnalyzer::~SpectrumAnalyzer()
{
    TimeSliceThread.removeTimeSliceClient(this);
}

//--------------------------------------------------------------------------------

int SpectrumAnalyzer::useTimeSlice()
{
    if (m_audioFifo.getAvailableSamples() > 0)
    {
        const int size{ std::min(m_buffers.front().getSize(), m_audioFifo.getAvailableSamples()) };

        m_conduitBuffer.clear();
        m_conduitBuffer.assign(static_cast<std::size_t>(size), 0.0f);

        m_audioFifo.popAudioData(m_conduitBuffer);

        for(auto &b : m_buffers)
        {
            b.write(m_conduitBuffer);

            if (b.isFftSpanReady())
            {
                auto fftSpan{ b.getFftSpan() };

                m_windowingFunction.multiplyWithWindowingTable(fftSpan.data(), std::min(fftSpan.size(), static_cast<std::size_t>(m_fft.getSize())));
                m_fft.performFrequencyOnlyForwardTransform(fftSpan.data());

                m_analyzerCurve.pushCurve(fftSpan);
            }
        }
    }

    return 1;
}

//--------------------------------------------------------------------------------

void SpectrumAnalyzer::stopTimeSliceThread()
{
    TimeSliceThread.stopThread(100);
}

//--------------------------------------------------------------------------------

SpectrumAnalyzerCurve::LockedCurve SpectrumAnalyzer::getAnalyzerCurve()
{
    m_analyzerCurve.setSampleRate(m_audioFifo.getSampleRate());

    return m_analyzerCurve.getCurve();
}

//--------------------------------------------------------------------------------
