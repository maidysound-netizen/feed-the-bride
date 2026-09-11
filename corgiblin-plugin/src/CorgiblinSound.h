#pragma once
#include <JuceHeader.h>

class CorgiblinSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override    { return true; }
    bool appliesToChannel (int) override { return true; }
};
