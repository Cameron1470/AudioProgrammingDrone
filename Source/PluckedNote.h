/*
  ==============================================================================

    PluckedNote.h
    Created: 23 Feb 2021 6:04:44pm
    Author:  csmit

  ==============================================================================
*/

#pragma once

#include <tgmath.h>
#include <JuceHeader.h>

class PluckedNote
{
public:
    PluckedNote() 
    {
        generateNote();
    }
    
    ~PluckedNote() {}

    //=====================================================================
    // GENERATE NOTE
    
    void generateNote()
    {
        //=======================================================================
        // KARPLUS-STRONG ALGORITHM

        // parameter calculation
        float rho = exp(-1 / ((float)frequency * T60 / log(1000))) / (abs(cos(2 * juce::double_Pi * frequency / sampleRate)));

        wtSize = floor(sampleRate * T60);               // duration of simulation in samples

        float Nexact = (sampleRate/frequency) - 0.5f;   // ideal number of samples in delay line
        float N = floor(Nexact);                        // truncated delay line length
        float P = Nexact - N;                           // fractional delay length
        float C = (1 - P) / (1 + P);                    // calculate allpass filter coefficient

        srand((unsigned)time(0));                       //random seed
        juce::Array<float> v;                           //initialize input vector

        // fill input vector with white noise
        for (int count = 0; count < N + 1; count++)
        {
            float randNum = ((rand() % 10001) / 5000.0f) - 1.0f;
            v.insert(count, randNum);
        }

        int x0;
        int x1 = 0;
        juce::Array<float> y;
        

        // dynamics filter loop
        for (int n = 0; n < (N + 1); n++)
        {
            x0 = (1 - dynParam) * v[(int) n] + (dynParam * x1);
            y.insert(n, x0);
            x1 = x0;
        }

        int yp1 = 0;                                // initializing previous output of allpas filter
        int yp0;                                    // initializing current output of allpass filter

        // karplus-strong algorithm loop
        for (int n = (N + 1); n < wtSize; n++)
        {
            yp0 = C * (y[int(n - N)] - yp1) + y[int(n - N - 1)];
            y.insert(n, (rho / 2) * (yp0 + yp1));
            yp1 = yp0;
        }

        // store generated note data in private variable
        waveTable = y;
    }

    //=============================================================================
    // PROCESS FUNCTION

    float process()
    {
        float sample = waveTable[(int)phase];
        
        phase += 1.0f;

        if (phase > wtSize)
            phase = 0.0f;

        return sample;

    }

    //=============================================================================
    // SETTER FUNCTIONS

    void setFrequency(float freq)
    {
        frequency = freq;
    }

    void setSampleRate(float SR)
    {
        sampleRate = SR;
    }

    void setNoteLength(float noteLength)
    {
        T60 = noteLength;
    }

private:
    float frequency = 440.0f;                       // frequency of plucked note variable
    float sampleRate = 48000.0f;                    // stored sample rate
    float dynParam = 0.95f;                         // dynamic parameter
    float T60 = 2.0f;                               // length of note in seconds

    float phase = 0.0f;                             // initialize phase                                      

    juce::Array<float> waveTable;                   // storing note data
    float wtSize = floor(sampleRate * T60);         // length in samples

};