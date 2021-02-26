/*
  ==============================================================================

    GenerativePlucks.h
    Created: 26 Feb 2021 1:42:25pm
    Author:  csmit

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluckedNote.h"

///  class for randomly playing karplus-strong notes from a vector of PluckedNote class
class GenerativePlucks
{
public:
    GenerativePlucks();
    ~GenerativePlucks();

    //=============================================================================
    /// generate wavetables for all plucked note types
    void generateNotes();
    /// return next value
    float processChord();
    //=============================================================================
#pragma mark getters and setters

    /// setter function for frequency
    /// @param freq, frequency of output note
    void setMidiNotes(int* midiNoteValues);
    /// setter function for sample rate
    /// @param SR, sample rate in Hz
    void setSampleRate(float SR);
    /// Time for note to decay
    /// @param noteLength, note length in seconds
    void setNoteLength(float noteLength);

private:
    /// frequency of plucked note variable
    std::vector<PluckedNote> plucks;
    /// number of note that make up the vector
    int noteCount = 5;

    float* frequencies = new float[noteCount];
    bool* noteStatus = new bool[noteCount];
    float* pause = new float[noteCount];
    float* timeStop = new float[noteCount];

    float sumSample = 0;

    juce::Random chance;

};

