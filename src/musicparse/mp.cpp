#include <cassert>
#include <iostream>
#include <SFML/Audio.hpp>
#include <aubio/aubio.h>

#include "mp.hpp"

using namespace std;

#define ONSET_DETECT "mkl"

Audio::Audio(const char *path)
{
  if (!sb_.loadFromFile(path)) {
    ready_ = false;
    return;
  }

  cout << "Loaded audio file with " <<
    sb_.getSampleCount() << " samples." << endl;
  cout << "File is " << sb_.getDuration().asSeconds() <<
    " seconds long, with " << sb_.getChannelCount() << " channels." << endl;
  sound_.setBuffer(sb_);
  ready_ = true;
}

Audio::~Audio()
{
  // Nada
}

void Audio::play()
{
  sound_.play();
}

float Audio::curTime()
{
  return sound_.getPlayingOffset().asSeconds();
}

std::vector<Beat> Audio::getBeats()
{
  uint_t win_s = 1024;
  uint_t hop_s = win_s / 4;

  const short *samples = sb_.getSamples();
  int num_samples = sb_.getSampleCount();
  int samplerate = sb_.getSampleRate();
  int num_channels = sb_.getChannelCount();

  assert (num_samples % num_channels == 0);

  aubio_onset_t *ons[num_channels];
  for (int i = 0; i < num_channels; i++)
    ons[i]  = new_aubio_onset(ONSET_DETECT, win_s, hop_s, samplerate);

  fvec_t * in  = new_fvec(hop_s);
  fvec_t * out = new_fvec(1);

  int num_hops = num_samples / hop_s;
  cout << num_hops << " hops left." << endl;

  int processed_hops = 0;
  int lastp = 0;

  vector<Beat> beats;

  uint step = hop_s * num_channels;
  for (uint i = 0; i < num_samples - step; i += step) {
    // Load input data
    for (int channel = 0; channel < num_channels; channel++) {
      for (uint j = 0; j < hop_s; j++) {
	in->data[j] = samples[i + j * num_channels + channel];
      }
      aubio_onset_do(ons[channel], in, out);

      // Find onset
      if (out->data[0]) {
	Beat b;

	b.samplenum = aubio_onset_get_last(ons[channel]);
	b.time = aubio_onset_get_last_s(ons[channel]);
	b.channel = channel;
	beats.push_back(b);
      }
    
      // Progress
      int prog = 10 * (++processed_hops) / num_hops;
      if (prog > lastp) {
	lastp = prog;
	cout << prog << "0% done." << endl;
      }
    }
  }

  for (int i = 0; i < num_channels; i++)
    del_aubio_onset(ons[i]);
  del_fvec(in);
  del_fvec(out);

  return beats;
}
