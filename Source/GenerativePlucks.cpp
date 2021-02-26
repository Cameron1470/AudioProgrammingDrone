/*
  ==============================================================================

    GenerativePlucks.cpp
    Created: 26 Feb 2021 1:42:25pm
    Author:  csmit

  ==============================================================================
*/

#include "GenerativePlucks.h"

GenerativePlucks::GenerativePlucks()
{
    for (int plucksIndex = 0; plucksIndex < 5; plucksIndex++)
    {
        plucks.push_back(PluckedNote());
    }
}
//=======================================================================
GenerativePlucks::~GenerativePlucks()
{}
//=======================================================================
void GenerativePlucks::generateNotes()
{
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].generateNote();
    }
}

//=============================================================================
// PROCESS FUNCTION
float GenerativePlucks::process()
{
    float sample;
    
    for (int i = 0; i < noteCount; ++i)
    {
        sample += plucks[i].process();
    }

    return sample;
}

//=============================================================================
// SETTER FUNCTIONS

void GenerativePlucks::setMidiNotes(float* midiNoteValues)
{
    for (int i = 0; i < noteCount; ++i)
    {
        frequencies[i] = juce::MidiMessage::getMidiNoteInHertz(midiNoteValues[i]);
    }
    
}
void GenerativePlucks::setSampleRate(float SR)
{
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].setSampleRate(SR);
    }
}
void GenerativePlucks::setNoteLength(float noteLength)
{
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].setNoteLength(noteLength);
    }
}