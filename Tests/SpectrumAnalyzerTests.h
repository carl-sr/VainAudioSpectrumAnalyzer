#pragma once

//-----------------------------------------------------------------------------

#include <gtest/gtest.h>
#include "SpectrumAnalyzer/AudioBufferFifo.h"
#include "SpectrumAnalyzer/SpectrumAnalyzerBuffer.h"

//-----------------------------------------------------------------------------

inline void fillBuffer(juce::AudioBuffer<float> &b, const float f)
{
    for(int c = 0; c < b.getNumChannels(); ++c)
    {
        for(int s = 0; s < b.getNumSamples(); ++s)
        {
            b.getWritePointer(c)[s] = f;
        }
    }
}

//-----------------------------------------------------------------------------

struct AbfHelper
{
private:
    static std::vector<float> createOutputBuffer(std::size_t size)
    {
        std::vector<float> b;
        b.insert(b.begin(), size, 0.0f);
        return b;
    }

    std::vector<float> m_outputBuffer;

public:
    explicit AbfHelper(const int bufferSize = 128, const int fifoSizeFactor = 2)
        : m_outputBuffer{ createOutputBuffer(static_cast<std::size_t>(bufferSize)) }
        , outputBuffer{ m_outputBuffer }
    {
        inputBuffer.setSize(2, bufferSize);
        fifo.setSize(bufferSize * fifoSizeFactor, 1.0);

        fillBuffer(inputBuffer, 0.0f);
    }

    std::span<float> getOutputBuffer(int length)
    {
        return { m_outputBuffer.begin(), m_outputBuffer.begin() + length };
    }

    juce::AudioBuffer<float> inputBuffer;
    AudioBufferFifo fifo;
    std::span<float> outputBuffer;


};

//-----------------------------------------------------------------------------

TEST(SpectrumAnalyzerTests, AudioBufferFifoCorrectValidityTest)
{
    constexpr int NumSamples = 100;
    constexpr int Scale = 2;

    AbfHelper h{ NumSamples, Scale };

    for(int i = 0; i < NumSamples; ++i)
    {
        h.inputBuffer.getWritePointer(0)[i] = static_cast<float>(i);
        h.inputBuffer.getWritePointer(1)[i] = static_cast<float>(i);
    }

    h.fifo.pushAudioData(h.inputBuffer);
    EXPECT_EQ(h.fifo.getAvailableSamples(), NumSamples);
    h.fifo.popAudioData(h.outputBuffer);

    for(std::size_t i = 0; i < static_cast<std::size_t>(NumSamples); ++i)
    {
        EXPECT_EQ(h.inputBuffer.getReadPointer(0)[i], h.outputBuffer[i]);
    }
}

//-----------------------------------------------------------------------------

TEST(SpectrumAnalyzerTests, AudioBufferFifoTearTest)
{
    constexpr int NumSamples = 100;
    constexpr int Scale = 1;

    AbfHelper h{ NumSamples, Scale };

    h.inputBuffer.setSize(2, 60); // 60, 60 will wrap around

    // inserting 60 samples will fill 60% of the fifo
    fillBuffer(h.inputBuffer, 1.0f);
    h.fifo.pushAudioData(h.inputBuffer);

    EXPECT_EQ(h.fifo.getAvailableSamples(), 60);

    h.fifo.popAudioData(h.outputBuffer);

    for(const auto &f : h.getOutputBuffer(60))
        EXPECT_EQ(f, 1.0f);

    // inserting another 60 samples should fill the last 40% and wrap to the first 20% of the fifo
    fillBuffer(h.inputBuffer, 2.0f);
    h.fifo.pushAudioData(h.inputBuffer);

    EXPECT_EQ(h.fifo.getAvailableSamples(), 60);

    h.fifo.popAudioData(h.outputBuffer);

    for(const auto &f : h.getOutputBuffer(60))
        EXPECT_EQ(f, 2.0f);
}

//-----------------------------------------------------------------------------

TEST(SpectrumAnalyzerTests, SpectrumAnalyzerBufferBasicTest)
{
    SpectrumAnalyzerBuffer b;
    b.setSize(10);

    std::vector<float> v{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f };

    b.write(v);
    b.write(v);

    EXPECT_TRUE(b.isFftSpanReady());

    auto fftSpan{ b.getFftSpan() };
    for(std::size_t i = 0 ; i < 10; ++i)
    {
        EXPECT_EQ(fftSpan[i], v[i % v.size()]);
    }
}

//-----------------------------------------------------------------------------

TEST(SpectrumAnalyzerTests, SpectrumAnalyzerBufferLoopTest)
{
    SpectrumAnalyzerBuffer b;
    b.setSize(5);

    std::vector<float> v{ 0.0f, 1.0f, 2.0f, 3.0f };

    b.write(v);
    b.write(v);

    EXPECT_TRUE(b.isFftSpanReady());

    {
        const std::vector<float> expect{ 0.0f, 1.0f, 2.0f, 3.0f, 0.0f };

        auto fftSpan{ b.getFftSpan() };
        for(const auto [i, f] : juce::enumerate(expect))
        {
            EXPECT_EQ(fftSpan[static_cast<std::size_t>(i)], f);
        }
    }

    EXPECT_FALSE(b.isFftSpanReady());
    b.skip(3);
    EXPECT_TRUE(b.isFftSpanReady());

    {
        const std::vector<float> expect{ 1.0f, 2.0f, 3.0f, 0.0f, 0.0f };

        auto fftSpan{ b.getFftSpan() };
        for(const auto [i, f] : juce::enumerate(expect))
        {
            EXPECT_EQ(fftSpan[static_cast<std::size_t>(i)], f);
        }
    }
}

//-----------------------------------------------------------------------------

TEST(SpectrumAnalyzerTests, SpectrumAnalyzerBufferTest)
{
    constexpr int Size{ 10 };
    SpectrumAnalyzerBuffer b;

    b.setSize(Size);

    float asdf{ 0.0f };
    std::vector<float> v;

    for(int i = 0; i < Size; ++i)
    {
        v.push_back(asdf);
        asdf += 1.0f;
    }

    b.write(v);

    auto fftSpan{ b.getFftSpan() };
    EXPECT_EQ(fftSpan.size(), Size);

    float fuck{ 0.0f };
    int loopCount2{ Size };
    for(auto f : fftSpan)
    {
        loopCount2--;
        EXPECT_EQ(fuck, f);
        fuck += 1.0f;
    }
    EXPECT_EQ(loopCount2, 0);
}

//-----------------------------------------------------------------------------


TEST(SpectrumAnalyzerTests, SpectrumAnalyzerBufferTearTest)
{
    constexpr int Size{ 10 };
    SpectrumAnalyzerBuffer b;

    b.setSize(Size);

    b.skip(Size / 2);

    std::vector<float> asdf;

    for(int i = 0; i < Size; ++i)
        asdf.push_back(static_cast<float>(i));

    b.write(asdf);

    auto r1{ b.getFftSpan() };

    std::span<const float> endSpan = { r1.begin() + Size / 2, r1.end() };

    float shit{ 0.0f };

    for(auto f : endSpan)
    {
        EXPECT_EQ(shit, f);
        shit += 1.0f;
    }

    EXPECT_FALSE(b.isFftSpanReady());

    std::vector<float> fuck;
    fuck.assign(Size / 2, 0.0f);
    b.write(fuck);

    EXPECT_TRUE(b.isFftSpanReady());

    auto r2{ b.getFftSpan() };
    std::span<const float> beginSpan = { r2.begin(), r2.begin() + Size / 2 };
    for(auto f : beginSpan)
    {
        EXPECT_EQ(shit, f);
        shit += 1.0f;
    }
}

//-----------------------------------------------------------------------------


TEST(SpectrumAnalyzerTests, SpectrumAnalyzerBufferWriteFromFifoTest)
{
    constexpr int Size{ 10 };

    SpectrumAnalyzerBuffer b;
    b.setSize(Size);
    AudioBufferFifo fifo;
    juce::AudioBuffer<float> buffer{ 2, Size };
    fillBuffer(buffer, 1.0f);

    fifo.setSize(100.0, 1.0);

    fifo.pushAudioData(buffer);

    std::vector<float> v;
    v.assign(static_cast<std::size_t>(fifo.getAvailableSamples()), 0.0f);
    fifo.popAudioData(v);
    b.write(v);

    for(auto f : b.getFftSpan())
        EXPECT_EQ(f, 1.0f);
}

//-----------------------------------------------------------------------------
