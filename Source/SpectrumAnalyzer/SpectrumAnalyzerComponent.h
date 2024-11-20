#pragma once

//-------------------------------------------------------------------------------------------------------

#include <juce_gui_basics/juce_gui_basics.h>
#include "SpectrumAnalyzer.h"

//---------------------------------------------------------------------------------------------------

class SpectrumAnalyzerComponent
    : public juce::Component
    , private juce::Timer
{
public:
    explicit SpectrumAnalyzerComponent(AudioBufferFifo &source);
    ~SpectrumAnalyzerComponent() override;

    void paint(juce::Graphics &g) override;

private:
    // juce::Timer
    void timerCallback() override;

    void paintAnalyzerCurve(juce::Graphics &g, const juce::Path &p);

    SpectrumAnalyzer m_fft;
    juce::Path m_curvePath;
    const juce::Colour m_colour{ juce::Colours::red };
};

//---------------------------------------------------------------------------------------------------
