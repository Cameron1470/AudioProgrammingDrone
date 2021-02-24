/*
   ==============================================================================

   PluckedNote.cpp
   Created: 23 Feb 2021 6:04:44pm
   Author:  csmit

   ==============================================================================
 */

#include "PluckedNote.cpp"
//=======================================================================
PluckedNote::PluckedNote()
{
  generateNote();
}
//=======================================================================
PluckedNote::~PluckedNote()
{}
//=======================================================================
void PluckedNote::generateNote()
{
  //=======================================================================
  // KARPLUS-STRONG ALGORITHM

  // parameter calculation
  float rho = exp(-1 / ((float)frequency * T60 / log(1000))) / (abs(cos(2 * juce::double_Pi * frequency / sampleRate)));

  wtSize = floor(sampleRate * T60);               // duration of simulation in samples

  float Nexact = (sampleRate / frequency) - 0.5f;   // ideal number of samples in delay line
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

float PluckedNote::process()
{
  float sample = waveTable[(int)phase];

  phase += 1.0f;

  if (phase > wtSize)
    phase = 0.0f;

  return sample;

}
//=============================================================================
// SETTER FUNCTIONS

void PluckedNote::setFrequency(float freq)
{
  frequency = freq;
}
void PluckedNote::setSampleRate(float SR)
{
  sampleRate = SR;
}
void PluckedNote::setNoteLength(float noteLength)
{
  T60 = noteLength;
}
