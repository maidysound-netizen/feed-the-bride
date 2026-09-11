#include "PluginEditor.h"
#include "ParameterIDs.h"

CorgiblinRecorderAudioProcessorEditor::CorgiblinRecorderAudioProcessorEditor (
    CorgiblinRecorderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (900, 610);

    title.setText ("CORGIBLIN RECORDER  JUCE BUILD 0.3", juce::dontSendNotification);
    title.setFont (juce::Font (juce::FontOptions (26.0f, juce::Font::bold)));
    title.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (title);

    oscLabel.setText ("OSCILLATOR", juce::dontSendNotification);
    oscLabel.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (oscLabel);

    waveform.addItemList ({ "Sine", "Saw", "Square", "Triangle" }, 1);
    addAndMakeVisible (waveform);
    waveA = std::make_unique<CA> (processor.apvts, ParamIDs::waveform, waveform);

    struct K
    {
        juce::Slider* s;
        juce::Label* l;
        const char* name;
        const char* id;
        const char* suffix;
    };

    K ks[] = {
        { &attack, &attackL, "ATTACK", ParamIDs::attack, " s" },
        { &decay, &decayL, "DECAY", ParamIDs::decay, " s" },
        { &sustain, &sustainL, "SUSTAIN", ParamIDs::sustain, "" },
        { &release, &releaseL, "RELEASE", ParamIDs::release, " s" },
        { &cutoff, &cutoffL, "CUTOFF", ParamIDs::cutoff, " Hz" },
        { &resonance, &resonanceL, "RESONANCE", ParamIDs::resonance, "" },
        { &heckLevel, &heckLevelL, "HECK", ParamIDs::heckLevel, "" },
        { &heckTune, &heckTuneL, "HECK TUNE", ParamIDs::heckTune, " st" },
        { &drive, &driveL, "DRIVE", ParamIDs::drive, " x" },
        { &delayMix, &delayMixL, "DELAY", ParamIDs::delayMix, "" },
        { &delayTime, &delayTimeL, "TIME", ParamIDs::delayTime, " s" },
        { &feedback, &feedbackL, "FEEDBACK", ParamIDs::feedback, "" },
        { &reverbMix, &reverbMixL, "REVERB", ParamIDs::reverbMix, "" },
        { &master, &masterL, "MASTER", ParamIDs::master, " dB" }
    };

    for (auto& k : ks)
    {
        setupKnob (*k.s, *k.l, k.name, k.suffix);
        attachments.push_back (std::make_unique<SA> (processor.apvts, k.id, *k.s));
    }
}

void CorgiblinRecorderAudioProcessorEditor::setupKnob (
    juce::Slider& s,
    juce::Label& l,
    const juce::String& name,
    const juce::String& suffix)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 68, 20);
    s.setTextValueSuffix (suffix);
    l.setText (name, juce::dontSendNotification);
    l.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (s);
    addAndMakeVisible (l);
}

void CorgiblinRecorderAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (246, 239, 224));
    g.setColour (juce::Colour::fromRGB (38, 38, 38));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (12), 18.0f, 2.0f);
    g.setFont (14.0f);
    g.drawText ("SYNTH + HECK EXCITER + DRIVE + DELAY + REVERB",
                20, 565, getWidth() - 40, 24, juce::Justification::centred);
}

void CorgiblinRecorderAudioProcessorEditor::resized()
{
    title.setBounds (20, 15, getWidth() - 40, 40);
    oscLabel.setBounds (25, 72, 130, 20);
    waveform.setBounds (25, 96, 130, 30);

    struct P { juce::Slider* s; juce::Label* l; };
    P row1[] = {
        { &attack, &attackL }, { &decay, &decayL }, { &sustain, &sustainL },
        { &release, &releaseL }, { &cutoff, &cutoffL }, { &resonance, &resonanceL }
    };
    P row2[] = {
        { &heckLevel, &heckLevelL }, { &heckTune, &heckTuneL }, { &drive, &driveL },
        { &delayMix, &delayMixL }, { &delayTime, &delayTimeL }, { &feedback, &feedbackL }
    };

    auto layoutRow = [&] (P* row, int n, int y)
    {
        const int start = 175, w = 105, gap = 10;
        for (int i = 0; i < n; ++i)
        {
            const int x = start + i * (w + gap);
            row[i].l->setBounds (x, y, w, 20);
            row[i].s->setBounds (x, y + 20, w, 95);
        }
    };

    layoutRow (row1, 6, 145);
    layoutRow (row2, 6, 300);

    reverbMixL.setBounds (315, 455, 105, 20);
    reverbMix.setBounds (315, 475, 105, 80);
    masterL.setBounds (465, 455, 105, 20);
    master.setBounds (465, 475, 105, 80);
}
