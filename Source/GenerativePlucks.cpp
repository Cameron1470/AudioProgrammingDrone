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
        noteStatus[plucksIndex] = false;
        pause[plucksIndex] = 0.0f;
        timeStop[plucksIndex] = chance.nextFloat() * 20.0f * 48000.0f;
        
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
    
    for (int i = 0; i < noteCount; ++i)
    {
        if (noteStatus[i] == true) {
            sumSample += plucks[i].process() / noteCount;
            if ((plucks[i].getCurrentSampleIndex() + 2) > plucks[i].getWtSize())
            {
                noteStatus[i] = false;
                float chanceVal = chance.nextFloat();
                if (chanceVal > 0.925f)
                {
                    timeStop[i] = chanceVal * 1.0f * plucks[i].getSampleRate();
                }
                else
                {
                    timeStop[i] = 0.0f;
                }
                
            }
        }

        else if (noteStatus[i] == false) {
            pause[i]++;
            if (pause[i] > timeStop[i]) {
                noteStatus[i] = true;
                pause[i] = 0;
            }
        }
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