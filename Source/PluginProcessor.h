/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Oscillators.h"
#include "GenerativePlucks.h"

//==============================================================================
/**
*/
class ApDroneProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ApDroneProjectAudioProcessor();
    ~ApDroneProjectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    TriOsc bassOscOne;
    SinOsc bassOscTwo;

    float gain = 0.3f;

    SinOsc ampModOne;
    TriOsc ampModTwo;
    
    // Plucked Notes

    GenerativePlucks pluckedNotes;
    
    juce::Reverb pluckedVerb;
    juce::Reverb::Parameters pluckedVerbParams;

    // Swells

    std::vector<SinOsc> swellOne;
    std::vector<SquareOsc> swellTwo;
    std::vector<SinOsc> swellThree;
    int swellComponents = 3;

    std::vector<SinOsc> swellTwoPhaseMod;

    SinSqOsc swellAmpLeft;
    SinSqOsc swellAmpRight;
    TriOsc swellAmpMod;

    // Fade In

    int fadeCounter = 0;
    float fadeLength = 20.0f;
    int fadeLengthInSamples;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ApDroneProjectAudioProcessor)
};
