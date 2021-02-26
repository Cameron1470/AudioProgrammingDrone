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
    for (int plucksIndex = 0; plucksIndex < noteCount; plucksIndex++)
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
float GenerativePlucks::processChord()
{
    sumSample = 0;
    
    for (int i = 0; i < 5; ++i)
    {
        sumSample += plucks[i].process() / noteCount;
    }

    return sumSample;
}

//=============================================================================
// SETTER FUNCTIONS

void GenerativePlucks::setMidiNotes(int* midiNoteValues)
{
    for (int i = 0; i < noteCount; ++i)
    {
        frequencies[i] = float(juce::MidiMessage::getMidiNoteInHertz(midiNoteValues[i]));
        plucks[i].setFrequency(frequencies[i]);
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