#include "CorgiblinVoice.h"
#include "CorgiblinSound.h"

CorgiblinVoice::CorgiblinVoice (juce::AudioProcessorValueTreeState& state)
    : apvts (state)
{
    oscillator.initialise ([] (float x) { return std::sin (x); }, 128);
}

bool CorgiblinVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<CorgiblinSound*> (sound) != nullptr;
}

void CorgiblinVoice::prepare (double sampleRate, int samplesPerBlock, int outputChannels)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32> (outputChannels);

    oscillator.prepare (spec);
    filter.prepare (spec);
    filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    ampEnvelope.setSampleRate (sampleRate);
    heck.prepare (sampleRate);
    voiceBuffer.setSize (outputChannels, samplesPerBlock);
}

void CorgiblinVoice::startNote (int midiNoteNumber,
                                float velocity,
                                juce::SynthesiserSound*,
                                int)
{
    const auto frequency = static_cast<float> (
        juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber));

    oscillator.setFrequency (frequency, true);
    level = velocity;
    currentMidiNote = midiNoteNumber;
    heck.noteOn (midiNoteNumber, velocity,
                 apvts.getRawParameterValue (ParamIDs::heckTune)->load());

    updateParameters();
    ampEnvelope.noteOn();
}

void CorgiblinVoice::stopNote (float, bool allowTailOff)
{
    ampEnvelope.noteOff();
    heck.noteOff();

    if (! allowTailOff || ! ampEnvelope.isActive())
    {
        clearCurrentNote();
        ampEnvelope.reset();
    }
}

void CorgiblinVoice::updateParameters()
{
    const int wave = static_cast<int> (
        apvts.getRawParameterValue (ParamIDs::waveform)->load());

    switch (wave)
    {
        case 0: oscillator.initialise ([] (float x) { return std::sin (x); }, 128); break;
        case 1: oscillator.initialise ([] (float x) { return x / juce::MathConstants<float>::pi; }, 128); break;
        case 2: oscillator.initialise ([] (float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128); break;
        case 3: oscillator.initialise ([] (float x) {
                    return (2.0f / juce::MathConstants<float>::pi) * std::asin (std::sin (x));
                }, 128); break;
        default: break;
    }

    envelopeParameters.attack  = apvts.getRawParameterValue (ParamIDs::attack)->load();
    envelopeParameters.decay   = apvts.getRawParameterValue (ParamIDs::decay)->load();
    envelopeParameters.sustain = apvts.getRawParameterValue (ParamIDs::sustain)->load();
    envelopeParameters.release = apvts.getRawParameterValue (ParamIDs::release)->load();
    ampEnvelope.setParameters (envelopeParameters);

    filter.setCutoffFrequency (apvts.getRawParameterValue (ParamIDs::cutoff)->load());
    filter.setResonance (apvts.getRawParameterValue (ParamIDs::resonance)->load());
}

void CorgiblinVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                                      int startSample,
                                      int numSamples)
{
    if (! isVoiceActive())
        return;

    updateParameters();

    if (voiceBuffer.getNumSamples() < numSamples
        || voiceBuffer.getNumChannels() != outputBuffer.getNumChannels())
    {
        voiceBuffer.setSize (outputBuffer.getNumChannels(), numSamples, false, false, true);
    }

    voiceBuffer.clear();

    juce::dsp::AudioBlock<float> block (voiceBuffer);
    auto subBlock = block.getSubBlock (0, static_cast<size_t> (numSamples));
    juce::dsp::ProcessContextReplacing<float> oscContext (subBlock);
    oscillator.process (oscContext);

    const float heckLevel = apvts.getRawParameterValue (ParamIDs::heckLevel)->load();
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float h = heck.nextSample() * heckLevel;
        for (int channel = 0; channel < voiceBuffer.getNumChannels(); ++channel)
            voiceBuffer.addSample (channel, sample, h);
    }

    filter.process (oscContext);
    ampEnvelope.applyEnvelopeToBuffer (voiceBuffer, 0, numSamples);
    voiceBuffer.applyGain (level);

    for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        outputBuffer.addFrom (channel, startSample, voiceBuffer, channel, 0, numSamples);

    if (! ampEnvelope.isActive())
    {
        clearCurrentNote();
        ampEnvelope.reset();
    }
}
