#pragma once

//--------------------------------------------------------------------------------
/**
 * @class SpectrumAnalyzerCurve
 * @brief thread safe class for storing fft curve
 *
 * push raw fft in, get a smooth curve out
 *
*/
//--------------------------------------------------------------------------------

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <span>
#include "SpectrumAnalyzerAverager.h"
#include "SpectrumAnalyzerBinSmoother.h"

//--------------------------------------------------------------------------------

class SpectrumAnalyzerCurve
{
public:
    explicit SpectrumAnalyzerCurve(int fftSize);

    void pushCurve(std::span<const float> fftCurve);

    void setSampleRate(double sampleRate);

    using PointType = juce::Point<float>;
    class LockedCurve : public std::span<const PointType>
    {
    public:
        ~LockedCurve() = default;

    private:
        friend class SpectrumAnalyzerCurve;

        LockedCurve(std::span<const PointType> &&span, std::mutex &mutex)
            : std::span<const PointType>{ span }
            , m_guard{ mutex }
        {}

        std::lock_guard<std::mutex> m_guard;
    };

    LockedCurve getCurve();

private:
    float indexToX(const double index) const;
    static float binToY(float bin);
    void generateCurve();

    std::mutex m_lock;

    double m_sampleRate{ 0.0 };
    const double m_fftSize{ 0.0f };
    std::vector<PointType> m_curvePoints;

    std::vector<SpectrumAnalyzerBinSmoother> m_smoother;

    SpectrumAnalyzerAverager m_averager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzerCurve)
};

//--------------------------------------------------------------------------------
