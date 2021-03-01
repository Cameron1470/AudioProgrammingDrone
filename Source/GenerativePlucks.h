/*
  ==============================================================================

    GenerativePlucks.h
    Created: 26 Feb 2021 1:42:25pm
    Author:  Cameron Smith, UoE 1338237

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
    /// @param Midi value, Midi note value of output note
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

    /// pointer array for storing frequency values
    float* frequencies = new float[noteCount];          

    /// pointer array for storing on/off status of notes
    bool* noteStatus = new bool[noteCount];             
    
    /// pointer array for storing pause time between notes
    float* pause = new float[noteCount];                
    
    /// pointer array for storing stop times for notes
    float* timeStop = new float[noteCount];                                         

    /// random object for chance values
    juce::Random chance;                                
};

