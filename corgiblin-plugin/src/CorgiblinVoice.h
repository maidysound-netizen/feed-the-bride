#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"
#include "HeckLayer.h"

class CorgiblinVoice : public juce::SynthesiserVoice
{
public:
    explicit CorgiblinVoice (juce::AudioProcessorValueTreeState& state);

    bool canPlaySound (juce::SynthesiserSound* sound) override;
    void prepare (double sampleRate, int samplesPerBlock, int outputChannels);

    void startNote (int midiNoteNumber,
                    float velocity,
                    juce::SynthesiserSound* sound,
                    int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override;

private:
    void updateParameters();

    juce::AudioProcessorValueTreeState& apvts;
    juce::dsp::Oscillator<float> oscillator;
    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::ADSR ampEnvelope;
    juce::ADSR::Parameters envelopeParameters;
    juce::AudioBuffer<float> voiceBuffer;

    double currentSampleRate = 44100.0;
    float level = 0.0f;
    int currentMidiNote = 60;
    HeckLayer heck;
};
