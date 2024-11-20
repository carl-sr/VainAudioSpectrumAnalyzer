#pragma once

//--------------------------------------------------------------------------------
/**
 * @class SpectrumAnalyzerBinSmoother
 * @brief single coordinate on the analyzer curve.
 *
 * basically the same thing as a juce::SmoothedValue but with some specific
 * behavior that makes it look nicer in the spectrum analyzer
 *
*/
//--------------------------------------------------------------------------------

#include <juce_dsp/juce_dsp.h>

//--------------------------------------------------------------------------------

class SpectrumAnalyzerBinSmoother
{
public:
    void setTargetValue(float target);
    float getNextValue();

    bool isAtTarget() const;

private:
    float m_current{ 0.0f };
    float m_target{ 0.0f };

    static constexpr float MultiplierInit{ 0.01f };
    float m_mul{ MultiplierInit };
};

//--------------------------------------------------------------------------------
