#include "SpectrumAnalyzerComponent.h"
#include "SpectrumAnalyzer.h"

//---------------------------------------------------------------------------------------------------

SpectrumAnalyzerComponent::SpectrumAnalyzerComponent(AudioBufferFifo &source)
    : m_fft(source)
{
    startTimerHz(30);
}

//---------------------------------------------------------------------------------------------------

SpectrumAnalyzerComponent::~SpectrumAnalyzerComponent()
{
    stopTimer();
}

//---------------------------------------------------------------------------------------------------

void SpectrumAnalyzerComponent::timerCallback()
{
    repaint();
}

//---------------------------------------------------------------------------------------------------

void SpectrumAnalyzerComponent::paintAnalyzerCurve(juce::Graphics &g, const juce::Path &p)
{
    const auto gradient{
        juce::ColourGradient::vertical(
            juce::Colours::red,
            0.0f,
            juce::Colours::red.withAlpha(0.25f),
            static_cast<float>(getHeight())
        )
    };
    g.setGradientFill(gradient);

    g.fillPath(p);
}

//---------------------------------------------------------------------------------------------------

void SpectrumAnalyzerComponent::paint(juce::Graphics &g)
{
    m_curvePath.clear();

    const auto b{ getLocalBounds().toFloat() };
    const auto levels{ m_fft.getAnalyzerCurve() };

    const auto bottomLeft{ b.getBottomLeft() };
    m_curvePath.startNewSubPath(bottomLeft);

    for (const auto &point : levels)
    {
        const float y{ juce::jmap(1.0f - point.y, 0.0f, b.getHeight()) };
        const float x{ juce::jmap(point.x, 0.0f, b.getWidth()) };

        m_curvePath.lineTo(juce::Point{ x, y });
    }

    m_curvePath = m_curvePath.createPathWithRoundedCorners(50.0f);

    const auto bottomRight{ b.getBottomRight() };
    m_curvePath.lineTo(bottomRight);

    paintAnalyzerCurve(g, m_curvePath);
}

//---------------------------------------------------------------------------------------------------
