// Occillators.h

#pragma once

#define Oscillators_h

#include <cmath>

//PARENT PHASOR CLASS
class Phasor
{
public:

	// Our parent oscillator class does the key things required for most oscillators:
	// -- Handles phase
	// -- Handles setters and getters for frequency and sample rate

	// update the phase and output the next sample from the oscillator
	float process()
	{
		phase += phaseDelta;

		if (phase > 1.0f)
			phase -= 1.0f;

		return output(phase);
	}

	virtual float output(float p)
	{
		return p;
	}

	void setSampleRate(float SR)
	{
		sampleRate = SR;
	}

	void setFrequency(float freq)
	{
		frequency = freq;
		phaseDelta = frequency / sampleRate;
	}

	void setPhase(float p)
	{
		phase = p;
	}

	void incrementPhase(float inc)
	{
		phase += inc;
	}

private:
	/// frequency variable
	float frequency;

	/// sample rate variable
	float sampleRate;

	/// phase variable
	float phase = 0.0f;

	/// phase delta increment
	float phaseDelta;
};

//=======================================

//CHILD class, Triangle Oscillator
class TriOsc : public Phasor
{
	float output(float p) override
	{
		return fabsf(p - 0.5f) - 0.25f;
	}
};

//CHILD class, Sinusoid Oscillator
class SinOsc : public Phasor
{
	float output(float p) override
	{
		return std::sin(p * 2.0 * 3.14159);
	}
};

//CHILD class, Sin^2 Oscillator
class SinSqOsc : public Phasor
{
	float output(float p) override
	{
		return std::sin(p * 2.0 * 3.141592653) * std::sin(p * 2.0 * 3.141592653);
	}
};

//CHILD class
class Sin12Osc : public Phasor
{
	float output(float p) override
	{
		return pow(std::sin(p * 2.0 * 3.141592653), 12);
	}
};

//CHILD class, Square Oscillator
class SquareOsc : public Phasor
{
	float output(float p) override
	{
		float outVal = 0.5;
		if (p > pulseWidth)
			outVal = -0.5;
		return outVal;
	}

	void setPulseWidth(float pw)
	{
		pulseWidth = pw;
	}

private:
	// puslewidth variable
	float pulseWidth = 0.5f;
};