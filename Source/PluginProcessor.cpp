/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GenerativePlucks.h"
#include <iostream>
#include "DelayLine.h"

//==============================================================================
ApDroneProjectAudioProcessor::ApDroneProjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    for (int i = 0; i < swellComponents; ++i)
    {
        swellOne.push_back(SinOsc());
        swellTwo.push_back(SquareOsc());
        swellThree.push_back(SinOsc());
        swellTwoPhaseMod.push_back(SinOsc());
    }
}

ApDroneProjectAudioProcessor::~ApDroneProjectAudioProcessor()
{
}

//==============================================================================
const juce::String ApDroneProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ApDroneProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ApDroneProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ApDroneProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ApDroneProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ApDroneProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ApDroneProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ApDroneProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ApDroneProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void ApDroneProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ApDroneProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    bassOscOne.setSampleRate(sampleRate);
    bassOscOne.setFrequency(juce::MidiMessage::getMidiNoteInHertz(48.0f));
    ampModOne.setSampleRate(sampleRate);
    ampModOne.setFrequency(0.05f);

    bassOscTwo.setSampleRate(sampleRate);
    bassOscTwo.setFrequency(juce::MidiMessage::getMidiNoteInHertz(55.0f));
    ampModTwo.setSampleRate(sampleRate);
    ampModTwo.setFrequency(0.5f);
    ampModTwo.setPhase(0.5f);

    //===================================================================================
    // Plucked Notes Initializing
    int* midiNoteValues = new int[10];
    midiNoteValues[0] = 51;
    midiNoteValues[1] = 55;
    midiNoteValues[2] = 58;
    midiNoteValues[3] = 62;
    midiNoteValues[4] = 67;

    pluckedNotes.setSampleRate(sampleRate);
    pluckedNotes.setMidiNotes(midiNoteValues);
    pluckedNotes.setNoteLength(5.0f);
    pluckedNotes.generateNotes();

    pluckedVerbParams.dryLevel = 0.5f;
    pluckedVerbParams.wetLevel = 0.5f;
    pluckedVerbParams.roomSize = 0.9f;
    pluckedVerb.setParameters(pluckedVerbParams);
    pluckedVerb.reset();

    delayedPlucks.setSizeInSamples(int(10.0f * sampleRate));
    delayedPlucks.setDelayTimeInSamples(int(0.3f * sampleRate));

    //====================================================================================
    // Swell Initializing
    
    for (int i = 0; i < swellComponents; ++i)
    {
        swellOne[i].setSampleRate(sampleRate);
        swellOne[i].setFrequency(juce::MidiMessage::getMidiNoteInHertz(36.0f) + ((float)-0.25f * (float)i));
        swellTwo[i].setSampleRate(sampleRate);
        swellTwo[i].setFrequency(juce::MidiMessage::getMidiNoteInHertz(55.0f) + ((float)-1.0f * (float)i));
        swellThree[i].setSampleRate(sampleRate);
        swellThree[i].setFrequency(juce::MidiMessage::getMidiNoteInHertz(24.0f) + ((float)-0.125f * (float)i));
        
        swellTwoPhaseMod[i].setSampleRate(sampleRate);
    }

    swellAmpLeft.setSampleRate(sampleRate);
    swellAmpRight.setSampleRate(sampleRate);

    swellAmpMod.setSampleRate(sampleRate);

    swellAmpMod.setFrequency(1.0f / 480.0f);
    swellAmpMod.setPhase(0.5f);

    // Fade In Initializing

    fadeLengthInSamples = float(sampleRate * fadeLength);
    
}

void ApDroneProjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ApDroneProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ApDroneProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //=====================================================================================================



    int numSamples = buffer.getNumSamples();            // store the number of sample in buffer
    float* leftChannel = buffer.getWritePointer(0);     // float pointer for writing to left channel
    float* rightChannel = buffer.getWritePointer(1);    // float pointer for writing to right channel

    //==============================================
    // DSP LOOP (with reverb)!
    for (int i = 0; i < numSamples; i++)
    {
        //==========================================
        // GENERATIVE KARPLUS-STRONG NOTES (RANDOM BEHAVIOUR)

        float genPlucks = pluckedNotes.processChord();  // get next sample of plucked notes

        // waiting for fade in to finish before introducing delay
        if (fadeCounter < fadeLengthInSamples)
        {
            // still processing and adding to delay buffer but not not adding to output signal
            delayedPlucks.process(genPlucks);
            leftChannel[i] = genPlucks * ksGain;           
            rightChannel[i] = genPlucks * ksGain;
        }
        else
        {
            // now adding to output signal
            leftChannel[i] = (genPlucks + delayedPlucks.process(genPlucks)) * ksGain;   
            rightChannel[i] = (genPlucks + delayedPlucks.process(genPlucks)) * ksGain;
        }
            
    }

    // apply reverb only to the plucked note sounds
    pluckedVerb.processStereo(leftChannel, rightChannel, numSamples);

    //==============================================
    // DSP LOOP (without reverb!)
    for (int i = 0; i < numSamples; i++)
    {
        //==========================================
        // BASS DRONE

        float modOne = ampModOne.process();             // get next sample of amplitude modulator one
        float modTwo = ampModTwo.process();             // get next sample of amplitude modulator two

        // Modulate frequency of bass oscillator one
        bassOscOne.setFrequency(juce::MidiMessage::getMidiNoteInHertz(48.0f) + 3.0 * modTwo);

        // Combine two bass note into master variable
        float bassMaster = modOne * bassOscOne.process() + modTwo * bassOscTwo.process();


        //==========================================
        // STEREO SWELLS

        float leftSwellSum = 0.0f;                      // initialize left channel swell sum float
        float rightSwellSum = 0.0f;                     // initialize right channel swell sum float

        // loop over all components of the vectors
        for (int i = 0; i < swellComponents; i++)
        {
            // left channel just takes values from first and seconds swell oscilattor vectors
            leftSwellSum += (swellOne[i].process() + swellTwo[i].process()) / 2.0f;

            // right channel just takes values from second and thirds swell oscillator vectors
            rightSwellSum += (swellTwo[i].process() + swellThree[i].process()) / 2.0f;
        }

        // update frequencies of amplitude modulation for both channels (notice different peak frequencies!)
        swellAmpLeft.setFrequency(0.01f + 0.99f * (0.25 + swellAmpMod.process()));
        swellAmpRight.setFrequency(0.01f + 0.90f * (0.25 + swellAmpMod.process()));
        
        // modulate amplitudes of both channels
        leftSwellSum *= swellAmpLeft.process() * 0.05f;
        rightSwellSum *= swellAmpRight.process() * 0.05f;

        // add bass and panned swells to the channel pointers
        leftChannel[i] += (bassMaster + leftSwellSum) * gain;
        rightChannel[i] += (bassMaster + rightSwellSum) * gain;
        
        //==========================================
        // FADE IN

        // will only be used for the initial fade time specified
        if (fadeCounter < fadeLengthInSamples)
        {
            // calculate current volume as a ratio of counter to length
            float fadeVolume = float(fadeCounter) / float(fadeLengthInSamples);

            leftChannel[i] *= fadeVolume;       // apply fade in to left channel
            rightChannel[i] *= fadeVolume;      // apply fade in to right channel
            fadeCounter++;                      // add 1 to counter
        }

    }
}

//==============================================================================
bool ApDroneProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ApDroneProjectAudioProcessor::createEditor()
{
    return new ApDroneProjectAudioProcessorEditor (*this);
}

//==============================================================================
void ApDroneProjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ApDroneProjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ApDroneProjectAudioProcessor();
}
