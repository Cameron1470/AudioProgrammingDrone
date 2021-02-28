/*
  ==============================================================================

    DelayLine.h
    Created: 28 Feb 2021 1:26:50pm
    Author:  csmit

  ==============================================================================
*/

#pragma once

class DelayLine
{
public:

    ~DelayLine()
    {
        delete[] buffer;
    }

    void setSizeInSamples(int newSizeInSamples)
    {
        size = newSizeInSamples;
        buffer = new float[size];
    }

    void setDelayTimeInSamples(int delayTimeInSamples)
    {
        readPos = writePos - delayTimeInSamples;
        if (readPos < 0)
            readPos += size;
    }

    float process(float inputSample)
    {
        buffer[writePos] = inputSample;

        writePos += 1;
        writePos %= size;

        float outputVal = buffer[readPos];

        readPos += 1;
        readPos %= size;

        return outputVal;
    }

    void setFeedback(float _feedback)
    {
        feedback = _feedback;
    }

private:
    int size;
    int readPos;
    int writePos = 0;

    float feedback;

    float* buffer;
};