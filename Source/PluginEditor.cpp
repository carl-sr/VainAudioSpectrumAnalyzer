#include "PluginEditor.h"

//-------------------------------------------------------------------------------------------------------

SpectrumAnalyzerAudioProcessorEditor::SpectrumAnalyzerAudioProcessorEditor(juce::AudioProcessor &p, AudioBufferFifo &source)
    : juce::AudioProcessorEditor(p)
    , m_analyzer(source)
{
    setSize(1000, 400);
    addAndMakeVisible(m_analyzer);
}

//-------------------------------------------------------------------------------------------------------

SpectrumAnalyzerAudioProcessorEditor::~SpectrumAnalyzerAudioProcessorEditor() = default;

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessorEditor::paint(juce::Graphics &g)
{
    constexpr juce::uint8 c{ 0x11 };
    g.fillAll(juce::Colour::fromRGB(c, c, c));
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessorEditor::resized()
{
    m_analyzer.setBounds(getLocalBounds());
}

//-------------------------------------------------------------------------------------------------------
