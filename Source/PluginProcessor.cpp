/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluckedNote.h"

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

    gNote.setFrequency(440.0f);
    gNote.setNoteLength(2.0f);
    gNote.setSampleRate(sampleRate);
    gNote.generateNote();

    
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

    int numSamples = buffer.getNumSamples();
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    //DSP LOOP!
    for (int i = 0; i < numSamples; i++)
    {
        //===================================================================================
        // BASS DRONE
        
        // time step the modulator variables
        float modOne = ampModOne.process();
        float modTwo = ampModTwo.process();

        // modulate
        bassOscOne.setFrequency(juce::MidiMessage::getMidiNoteInHertz(48.0f) + 3.0 * modTwo);
        
        float bassMaster = modOne * bassOscOne.process() + modTwo * bassOscTwo.process();


        //====================================================================================
        // GENERATIVE KARPLUS-STRONG HIGH NOTES

        float KSNote = gNote.process();

        //====================================================================================
        // PANNED NOISE BURSTS

        //====================================================================================
        // OUTPUT

        leftChannel[i] = (KSNote) * gain;
        rightChannel[i] = (KSNote) * gain;

        //leftChannel[i] = KSNote * gain;
        //rightChannel[i] = KSNote * gain;
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
