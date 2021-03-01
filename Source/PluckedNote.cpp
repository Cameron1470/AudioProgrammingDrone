/*
 ==============================================================================

 PluckedNote.cpp
 Created: 23 Feb 2021 6:04:44pm
 Author:  Cameron Smith, UoE 1338237

 ==============================================================================
 */

#include "PluckedNote.h"
//===========================================================================
PluckedNote::PluckedNote()
{}
//===========================================================================
PluckedNote::~PluckedNote()
{}
//===========================================================================
void PluckedNote::generateNote()
{
    //=======================================================================
    // KARPLUS-STRONG ALGORITHM

    // parameter calculation for use in KS algorithm
    float rho = exp(-1 / ((float)frequency * T60 / log(1000))) / (abs(cos(2 * 3.14159 * frequency / sampleRate)));

    wtSize = floor(sampleRate * T60);                   // duration of simulation in samples

    float Nexact = (sampleRate / frequency) - 0.5f;     // ideal number of samples in delay line
    int N = floor(Nexact);                              // truncated delay line length
    float P = Nexact - N;                               // fractional delay length
    float C = (1 - P) / (1 + P);                        // calculate allpass filter coefficient

    srand((unsigned)time(0));                           //random seed
    float* v = new float[N + 1];                        //initialize input vector

    // fill input vector with white noise
    for (int count = 0; count < N + 1; count++)
    {
        float randNum = ((rand() % 10001) / 5000.0f) - 1.0f;
        v[count] = randNum;
    }

    // initializing time steps
    float x0;                                          
    float x1 = 0;

    // if the wavetable is not a null pointer, delete contents (prevention of memory leakage, thanks Matthew)
    if (waveTable != nullptr)
    {
        delete[] waveTable;
    }
    // initializing wavetable array of wavetable size
    waveTable = new float[wtSize];

    // dynamics filter loop
    float record = 0.0f;
    for (int n = 0; n < (N + 1); n++)
    {
        x0 = (1 - dynParam) * v[n] + (dynParam * x1);
        waveTable[n] = x0;
        x1 = x0;
    }

    float yp1 = 0;                                // initializing previous output of allpas filter
    float yp0;                                    // initializing current output of allpass filter

    // karplus-strong algorithm loop
    for (int n = (N + 1); n < wtSize; n++)
    {
        yp0 = C * (waveTable[(n - N)] - yp1) + waveTable[(n - N - 1)];
        waveTable[n] = (rho * 0.5f) * (yp0 + yp1);
        yp1 = yp0;
    }


}
//=============================================================================
// PROCESS FUNCTION
float PluckedNote::process()
{
    float sample = waveTable[currentSampleIndex];
    currentSampleIndex++;
    currentSampleIndex %= wtSize;

    return sample;
}
//=============================================================================
// SETTER FUNCTIONS

void PluckedNote::setFrequency(float freq)
{
    // set private frequency variable with new value
    frequency = freq;
}
void PluckedNote::setSampleRate(float SR)
{
    // set private sample rate variable with new value
    sampleRate = SR;
}
void PluckedNote::setNoteLength(float noteLength)
{
    // set private T60 variable with new value
    T60 = noteLength;
}

//==============================================================================
// GETTER FUNCTIONs

int PluckedNote::getCurrentSampleIndex()
{
    // return the current sample index value
    return currentSampleIndex;
}

int PluckedNote::getWtSize()
{
    // return the wavetable size
    return wtSize;
}

float PluckedNote::getSampleRate()
{
    // return the sample rate
    return sampleRate;
}

