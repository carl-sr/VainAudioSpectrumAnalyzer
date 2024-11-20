#include "SpectrumAnalyzerBinSmoother.h"

//--------------------------------------------------------------------------------

void SpectrumAnalyzerBinSmoother::setTargetValue(const float target)
{
    m_target = target;
    if(target > m_current)
    {
        m_current = target;
        m_mul = MultiplierInit;
    }
}

//--------------------------------------------------------------------------------

float SpectrumAnalyzerBinSmoother::getNextValue()
{
    if(m_target < m_current)
    {
        m_mul += m_mul;
        m_current -= m_mul;
        m_current = std::max(m_target, m_current);
    }

    return m_current;
}

//--------------------------------------------------------------------------------

bool SpectrumAnalyzerBinSmoother::isAtTarget() const
{
    return juce::exactlyEqual(m_current, m_target);
}

//--------------------------------------------------------------------------------
