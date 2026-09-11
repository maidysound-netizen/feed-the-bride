#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "CorgiblinVoice.h"
#include "CorgiblinSound.h"
#include "ParameterIDs.h"

CorgiblinRecorderAudioProcessor::CorgiblinRecorderAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    for (int i = 0; i < 8; ++i)
        synth.addVoice (new CorgiblinVoice (apvts));
    synth.addSound (new CorgiblinSound());
}

juce::AudioProcessorValueTreeState::ParameterLayout
CorgiblinRecorderAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        ParamIDs::waveform, "Waveform", juce::StringArray { "Sine", "Saw", "Square", "Triangle" }, 0));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::attack, "Attack", juce::NormalisableRange<float> (0.001f, 3.0f, 0.001f, 0.35f), 0.01f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::decay, "Decay", juce::NormalisableRange<float> (0.001f, 3.0f, 0.001f, 0.35f), 0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::sustain, "Sustain", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.75f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::release, "Release", juce::NormalisableRange<float> (0.01f, 6.0f, 0.001f, 0.35f), 0.4f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::cutoff, "Cutoff", juce::NormalisableRange<float> (40.0f, 18000.0f, 1.0f, 0.25f), 6000.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::resonance, "Resonance", juce::NormalisableRange<float> (0.1f, 1.0f, 0.001f), 0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::master, "Master", juce::NormalisableRange<float> (-36.0f, 6.0f, 0.1f), -6.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::heckLevel, "HECK Level", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::heckTune, "HECK Tune", juce::NormalisableRange<float> (-24.0f, 24.0f, 1.0f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::drive, "Drive", juce::NormalisableRange<float> (1.0f, 12.0f, 0.01f), 1.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::delayMix, "Delay Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::delayTime, "Delay Time", juce::NormalisableRange<float> (0.02f, 1.0f, 0.001f), 0.25f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::feedback, "Delay Feedback", juce::NormalisableRange<float> (0.0f, 0.92f, 0.001f), 0.35f));
    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        ParamIDs::reverbMix, "Reverb Mix", juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    return { params.begin(), params.end() };
}

void CorgiblinRecorderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<CorgiblinVoice*> (synth.getVoice (i)))
            voice->prepare (sampleRate, samplesPerBlock, getTotalNumOutputChannels());

    juce::dsp::ProcessSpec spec {
        sampleRate,
        static_cast<juce::uint32> (samplesPerBlock),
        static_cast<juce::uint32> (getTotalNumOutputChannels())
    };
    delay.prepare (spec);
    delay.reset();
}

bool CorgiblinRecorderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void CorgiblinRecorderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                    juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    const float drive = apvts.getRawParameterValue (ParamIDs::drive)->load();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            buffer.setSample (ch, i, std::tanh (buffer.getSample (ch, i) * drive));

    const float delayMix = apvts.getRawParameterValue (ParamIDs::delayMix)->load();
    const float delaySeconds = apvts.getRawParameterValue (ParamIDs::delayTime)->load();
    const float feedback = apvts.getRawParameterValue (ParamIDs::feedback)->load();
    delay.setDelay (static_cast<float> (getSampleRate() * delaySeconds));

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const float dry = buffer.getSample (ch, i);
            const float wet = delay.popSample (ch);
            delay.pushSample (ch, dry + wet * feedback);
            buffer.setSample (ch, i, dry * (1.0f - delayMix) + wet * delayMix);
        }
    }

    juce::Reverb::Parameters rp;
    const float reverbMix = apvts.getRawParameterValue (ParamIDs::reverbMix)->load();
    rp.roomSize = 0.55f;
    rp.damping = 0.45f;
    rp.wetLevel = reverbMix;
    rp.dryLevel = 1.0f - reverbMix;
    rp.width = 1.0f;
    reverb.setParameters (rp);

    if (buffer.getNumChannels() >= 2)
        reverb.processStereo (buffer.getWritePointer (0), buffer.getWritePointer (1), buffer.getNumSamples());
    else if (buffer.getNumChannels() == 1)
        reverb.processMono (buffer.getWritePointer (0), buffer.getNumSamples());

    const auto masterDb = apvts.getRawParameterValue (ParamIDs::master)->load();
    buffer.applyGain (juce::Decibels::decibelsToGain (masterDb));
}

void CorgiblinRecorderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, destData);
}

void CorgiblinRecorderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* CorgiblinRecorderAudioProcessor::createEditor()
{
    return new CorgiblinRecorderAudioProcessorEditor (*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CorgiblinRecorderAudioProcessor();
}
