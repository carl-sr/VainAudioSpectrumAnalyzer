#include "SpectrumAnalyzerCurve.h"

//--------------------------------------------------------------------------------

static constexpr float MinFrequency{ 20.0f };
static constexpr float MaxFrequency{ 20'000.0f };

//--------------------------------------------------------------------------------

SpectrumAnalyzerCurve::SpectrumAnalyzerCurve(int fftSize)
    : m_fftSize{ static_cast<double>(fftSize) }
    , m_averager(fftSize)
{
}

//--------------------------------------------------------------------------------

void SpectrumAnalyzerCurve::pushCurve(std::span<const float> fftCurve)
{
    std::lock_guard guard{ m_lock };

    m_averager.pushCurve(fftCurve);
}

//--------------------------------------------------------------------------------

void SpectrumAnalyzerCurve::setSampleRate(double sampleRate)
{
    m_sampleRate = sampleRate;
}

//--------------------------------------------------------------------------------

static float getParameterValueFromFrequency(const float frequency)
{
    if (juce::exactlyEqual(frequency, MinFrequency))
        return 0.0f;

    jassert(frequency >= MinFrequency);
    jassert(frequency <= MaxFrequency);

    const float num = std::log(frequency - MinFrequency);
    const float den = std::log(MaxFrequency - MinFrequency);
    return std::powf(num / den, 2.0f);
}

//--------------------------------------------------------------------------------

float SpectrumAnalyzerCurve::indexToX(const double index) const
{
    const double freq{ (m_sampleRate * index / m_fftSize) };
    const float trimFreq = std::clamp(static_cast<float>(freq), MinFrequency, MaxFrequency);
    return getParameterValueFromFrequency(trimFreq);
}

//--------------------------------------------------------------------------------

float SpectrumAnalyzerCurve::binToY(float bin)
{
    constexpr float infinity = -80.0f;
    return juce::jmap(juce::Decibels::gainToDecibels(bin, infinity), infinity, 0.0f, 0.0f, 1.0f);
}

//--------------------------------------------------------------------------------

void SpectrumAnalyzerCurve::generateCurve()
{
    jassert(m_sampleRate > 0.0f);

    std::lock_guard guard{ m_lock };

    const std::span<const float> averagerSpan{ m_averager.getReadSpan() };

    if(m_smoother.size() != averagerSpan.size())
    {
        m_smoother.clear();
        m_smoother.assign(averagerSpan.size(), {});
    }

    m_curvePoints.clear();
    for(const auto [index, level] : juce::enumerate(averagerSpan))
    {
        const auto i{ static_cast<std::size_t>(index) };
        m_smoother[i].setTargetValue(binToY(level));

        const double x{ indexToX(static_cast<double>(index)) };
        const float y{ m_smoother[i].getNextValue() };
        const juce::Point<float> point{ static_cast<float>(x), y };
        m_curvePoints.push_back(point);
    }
}

//--------------------------------------------------------------------------------

SpectrumAnalyzerCurve::LockedCurve SpectrumAnalyzerCurve::getCurve()
{
    generateCurve();

    return LockedCurve{ m_curvePoints, m_lock };
}

//--------------------------------------------------------------------------------
