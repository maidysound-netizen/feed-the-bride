#pragma once
#include <JuceHeader.h>

class HeckLayer
{
public:
    void prepare (double newSampleRate)
    {
        sampleRate = newSampleRate;
        envelope.setSampleRate (sampleRate);
    }

    void noteOn (int midiNote, float velocity, float tuneSemitones)
    {
        const auto hz = juce::MidiMessage::getMidiNoteInHertz (midiNote + tuneSemitones);
        phaseDelta = juce::MathConstants<double>::twoPi * hz / sampleRate;
        level = velocity;
        phase = 0.0;

        juce::ADSR::Parameters p;
        p.attack = 0.001f;
        p.decay = 0.075f;
        p.sustain = 0.0f;
        p.release = 0.03f;
        envelope.setParameters (p);
        envelope.noteOn();
    }

    void noteOff() { envelope.noteOff(); }

    float nextSample()
    {
        if (! envelope.isActive())
            return 0.0f;

        const float fundamental = std::sin (phase) >= 0.0 ? 1.0f : -1.0f;
        const float overtone = static_cast<float> (0.35 * std::sin (phase * 3.0));
        phase += phaseDelta;
        if (phase >= juce::MathConstants<double>::twoPi)
            phase -= juce::MathConstants<double>::twoPi;

        return (fundamental * 0.65f + overtone) * envelope.getNextSample() * level;
    }

private:
    double sampleRate = 44100.0;
    double phase = 0.0, phaseDelta = 0.0;
    float level = 0.0f;
    juce::ADSR envelope;
};
