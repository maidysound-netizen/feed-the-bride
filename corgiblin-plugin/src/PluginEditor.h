#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class CorgiblinRecorderAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit CorgiblinRecorderAudioProcessorEditor (CorgiblinRecorderAudioProcessor&);
    ~CorgiblinRecorderAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setupKnob (juce::Slider&, juce::Label&, const juce::String&, const juce::String& suffix = {});

    CorgiblinRecorderAudioProcessor& processor;
    juce::Label title, oscLabel;
    juce::ComboBox waveform;

    juce::Slider attack, decay, sustain, release, cutoff, resonance, master;
    juce::Slider heckLevel, heckTune, drive, delayMix, delayTime, feedback, reverbMix;
    juce::Label attackL, decayL, sustainL, releaseL, cutoffL, resonanceL, masterL;
    juce::Label heckLevelL, heckTuneL, driveL, delayMixL, delayTimeL, feedbackL, reverbMixL;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<CA> waveA;
    std::vector<std::unique_ptr<SA>> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CorgiblinRecorderAudioProcessorEditor)
};
