/*
 ==============================================================================

 PluckedNote.h
 Created: 23 Feb 2021 6:04:44pm
 Author:  csmit

 ==============================================================================
 */

#pragma once

#include <tgmath.h>

 ///  class for playing karplus-strong notes using wavetable methods
class PluckedNote
{
public:
    PluckedNote();
    ~PluckedNote();

    //=============================================================================
    /// generate wavetable for class parameters
    void generateNote();
    /// return next value from the wavetable
    float process();
    //=============================================================================
#pragma mark getters and setters

    /// setter function for frequency
    /// @param freq, frequency of output note
    void setFrequency(float freq);
    /// setter function for sample rate
    /// @param SR, sample rate in Hz
    void setSampleRate(float SR);
    /// Time for note to decay
    /// @param noteLength, note length in seconds
    void setNoteLength(float noteLength);

private:
    /// frequency of plucked note variable
    float frequency = 440.0f;
    /// stored sample rate
    float sampleRate = 48000.0f;
    /// dynamic parameter
    float dynParam = 0.95f;
    /// length of note in seconds
    float T60 = 2.0f;
    /// initialize phase
    int currentSampleIndex = 0;
    /// storing note data
    float* waveTable;
    /// length in samples
    int wtSize = floor(sampleRate * T60);
};
