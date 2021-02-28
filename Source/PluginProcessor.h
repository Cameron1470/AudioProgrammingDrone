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
    //==========================================
    // Bass Oscillator Variables

    /// Bass oscillator one, triangle shape
    TriOsc bassOscOne;

    /// Bass oscillator two, sinusoidal shape
    SinOsc bassOscTwo;              

    /// Gain value used for reducing volume
    float gain = 0.3f;              

    /// Amplitude modulator one, sinusoidal shape
    SinOsc ampModOne;            

    /// Amplitude modulator two, triangle shaped
    TriOsc ampModTwo;               
    
    //==========================================
    // Plucked Note Variables

    /// Generates random plucked notes from Karplus-Strong
    GenerativePlucks pluckedNotes;  
    
    /// Reverb for use on plucked notes
    juce::Reverb pluckedVerb;    

    /// Stored parameters for reverb on plucked notes
    juce::Reverb::Parameters pluckedVerbParams;

    /// Gain reduction of Karplus-Strong Notes
    float ksGain = 0.6f;

    //==========================================
    // Swell Variables

    /// Swell one, vector of sinusoidal oscillators
    std::vector<SinOsc> swellOne;

    /// Swell two, vector of square oscillators
    std::vector<SquareOsc> swellTwo;

    /// Swell three, vector of sinusoidal oscillators
    std::vector<SinOsc> swellThree;

    /// Number of components to each swell vector
    int swellComponents = 3;

    std::vector<SinOsc> swellTwoPhaseMod;

    /// Swell amplitude modulation of left channel, sine squared shape
    SinSqOsc swellAmpLeft;

    /// Swell amplitude modulation of right channel, sine squared shape
    SinSqOsc swellAmpRight;

    /// Modulation of amplitude modulation frequency, triangle shaped
    TriOsc swellAmpMod;

    //==========================================
    // Fade In Variables

    /// Counter for fade in duration
    int fadeCounter = 0;   

    /// Length of fade in (in seconds)
    float fadeLength = 20.0f;     

    /// Length of fade in (in samples, calculated with sample rate)
    int fadeLengthInSamples;                


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ApDroneProjectAudioProcessor)
};
