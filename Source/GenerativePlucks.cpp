/*
  ==============================================================================

    GenerativePlucks.cpp
    Created: 26 Feb 2021 1:42:25pm
    Author:  Cameron Smith, UoE 1338237

  ==============================================================================
*/

#include "GenerativePlucks.h"

GenerativePlucks::GenerativePlucks()
{
    // loop across number of notes
    for (int plucksIndex = 0; plucksIndex < noteCount; plucksIndex++)
    {
        // add plucked note to array
        plucks.push_back(PluckedNote());
        
        // initialize all with off status
        noteStatus[plucksIndex] = false;
        
        // initialize all with pause value of 0
        pause[plucksIndex] = 0.0f;

        // initialize with random time stop value, will randomly start in the first 20 seconds
        timeStop[plucksIndex] = chance.nextFloat() * 20.0f * 48000.0f;
        
    }


}
//=======================================================================
GenerativePlucks::~GenerativePlucks()
{}
//=======================================================================
void GenerativePlucks::generateNotes()
{
    //loop across all notes and generate wavetable, stored in plucks array
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].generateNote();
    }
}

//=============================================================================
// PROCESS FUNCTION
float GenerativePlucks::processChord()
{
    // initialize a sum variable for each process call
    float sumSample = 0;
    
    // looping across all notes
    for (int i = 0; i < noteCount; ++i)
    {
        // if note status is TRUE ==> process and add to output
        if (noteStatus[i] == true) {
            sumSample += plucks[i].process() / noteCount;

            // if note has reached the end of its duration...
            if ((plucks[i].getCurrentSampleIndex() + 2) > plucks[i].getWtSize())
            {
                // change status to false
                noteStatus[i] = false;
                
                // get a new random value
                float chanceVal = chance.nextFloat();

                // in the occasional chance this value is high, perform a small time stop
                if (chanceVal > 0.925f)
                {
                    timeStop[i] = chanceVal * 1.0f * plucks[i].getSampleRate();
                }

                // let note continue following the rhythm of other notes
                else
                {
                    timeStop[i] = 0.0f;
                }
                
            }
        }

        // if note status is FALSE ==> dont add to output but increment pause
        else if (noteStatus[i] == false) {
            pause[i]++;

            // once pause is finished change note status to true and reset pause
            if (pause[i] > timeStop[i]) {
                noteStatus[i] = true;
                pause[i] = 0;
            }
        }
    }

    // return sum of samples
    return sumSample;
}

//=============================================================================
// SETTER FUNCTIONS

void GenerativePlucks::setMidiNotes(int* midiNoteValues)
{
    // loop across number of notes and modify frequencies
    for (int i = 0; i < noteCount; ++i)
    {
        // convert from midi value to frequency using juce function
        frequencies[i] = float(juce::MidiMessage::getMidiNoteInHertz(midiNoteValues[i]));
        plucks[i].setFrequency(frequencies[i]);
    }
    
}
void GenerativePlucks::setSampleRate(float SR)
{
    // loops across number of notes and set sample rate
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].setSampleRate(SR);
    }
}
void GenerativePlucks::setNoteLength(float noteLength)
{
    // loops across number of notes and set note length
    for (int i = 0; i < noteCount; ++i)
    {
        plucks[i].setNoteLength(noteLength);
    }
}