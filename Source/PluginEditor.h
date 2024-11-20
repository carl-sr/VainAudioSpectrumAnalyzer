#pragma once

//-------------------------------------------------------------------------------------------------------

#include <juce_audio_processors/juce_audio_processors.h>
#include <SpectrumAnalyzer/SpectrumAnalyzerComponent.h>

//-------------------------------------------------------------------------------------------------------

class SpectrumAnalyzerAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit SpectrumAnalyzerAudioProcessorEditor(juce::AudioProcessor &p, AudioBufferFifo &source);
    ~SpectrumAnalyzerAudioProcessorEditor() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

private:
    SpectrumAnalyzerComponent m_analyzer;
};

//-------------------------------------------------------------------------------------------------------

