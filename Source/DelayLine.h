/*
  ==============================================================================

    DelayLine.h
    Created: 28 Feb 2021 1:26:50pm
    Author:  Cameron Smith, UoE 1338237

  ==============================================================================
*/

#pragma once


/// Delay line class -- contains a buffer which can be read and written to at different points
class DelayLine
{
public:
    /// Destructor
    ~DelayLine()
    {
        delete[] buffer;                                // clear buffer from memory
    }


    /// Setter function for size of delay line
    /// @param newSizeInSamples, length of delay line in samples
    void setSizeInSamples(int newSizeInSamples)
    {
        size = newSizeInSamples;                        // set size private variable
        buffer = new float[size];                       // assign pointer with a new float array of size
    }

    /// Setter function for delay time in samples
    /// @param delayTimeInSamples, length of delay time in samples
    void setDelayTimeInSamples(int delayTimeInSamples)
    {
        readPos = writePos - delayTimeInSamples;        // set read position to be write position minus delay
        if (readPos < 0)
            readPos += size;                            // if < 0, add size of delay line - back in range
    }

    /// Input new value to buffer, returns output from delay
    float process(float inputSample)
    {
        buffer[writePos] = inputSample;                 //update buffer with new input sample

        writePos += 1;                                  // increment write position
        writePos %= size;                               // loop back around to zero if off the end of buffer

        float outputVal = buffer[readPos];              // read output value from read position of buffer

        readPos += 1;                                   // increment read position
        readPos %= size;                                // loop back around to zero if off the end of buffer

        return outputVal;                               // return output value
    }

    /// Setter function for the feedback of delay line
    void setFeedback(float _feedback)
    {
        feedback = _feedback;
    }

private:
    int size;                                           // size of delay line in samples
    int readPos;                                        // read position on buffer
    int writePos = 0;                                   // write position on buffer

    float feedback;                                     // feedback variable

    float* buffer;                                      // initializing pointer for buffer
};