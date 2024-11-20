#include "PluginProcessor.h"
#include "PluginEditor.h"

//-------------------------------------------------------------------------------------------------------

juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new SpectrumAnalyzerAudioProcessor();
}

//-------------------------------------------------------------------------------------------------------

SpectrumAnalyzerAudioProcessor::SpectrumAnalyzerAudioProcessor()
    : juce::AudioProcessor(juce::AudioProcessor::BusesProperties{}
                           .withInput("Input", juce::AudioChannelSet::stereo(), true)
                           .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

//-------------------------------------------------------------------------------------------------------

SpectrumAnalyzerAudioProcessor::~SpectrumAnalyzerAudioProcessor() = default;

//-------------------------------------------------------------------------------------------------------

const juce::String SpectrumAnalyzerAudioProcessor::getName() const
{
    return "Spectrum Analyzer";
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    (void)(maximumExpectedSamplesPerBlock);
    m_fifo.setSize(sampleRate, 1.0);
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::releaseResources()
{
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    (void)(buffer);
    (void)(midiMessages);
    m_fifo.pushAudioData(buffer);
}

//-------------------------------------------------------------------------------------------------------

double SpectrumAnalyzerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0f;
}

//-------------------------------------------------------------------------------------------------------

bool SpectrumAnalyzerAudioProcessor::acceptsMidi() const
{
    return false;
}

//-------------------------------------------------------------------------------------------------------

bool SpectrumAnalyzerAudioProcessor::producesMidi() const
{
    return false;
}

//-------------------------------------------------------------------------------------------------------

juce::AudioProcessorEditor *SpectrumAnalyzerAudioProcessor::createEditor()
{
    return new SpectrumAnalyzerAudioProcessorEditor(*this, m_fifo);
}

//-------------------------------------------------------------------------------------------------------

bool SpectrumAnalyzerAudioProcessor::hasEditor() const
{
    return true;
}

//-------------------------------------------------------------------------------------------------------

int SpectrumAnalyzerAudioProcessor::getNumPrograms()
{
    return 0;
}

//-------------------------------------------------------------------------------------------------------

int SpectrumAnalyzerAudioProcessor::getCurrentProgram()
{
    return 0;
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::setCurrentProgram(int index)
{
    (void)(index);
}

//-------------------------------------------------------------------------------------------------------

const juce::String SpectrumAnalyzerAudioProcessor::getProgramName(int index)
{
    (void)(index);
    return "null";
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::changeProgramName(int index, const juce::String &newName)
{
    (void)(index);
    (void)(newName);
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    (void)(destData);
}

//-------------------------------------------------------------------------------------------------------

void SpectrumAnalyzerAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    (void)(data);
    (void)(sizeInBytes);
}

//-------------------------------------------------------------------------------------------------------
