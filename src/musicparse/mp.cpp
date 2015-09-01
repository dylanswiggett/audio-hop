#include <cassert>
#include <algorithm>
#include <iostream>

#include <SFML/Audio.hpp>
#include <aubio/aubio.h>

#include "mp.hpp"

using namespace std;

#define ONSET_DETECT "mkl"
#define TEMPO_DETECT "default"

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

AudioMetadata Audio::getMetadata()
{
  if (data_.beats.size() > 0)
    return data_; // Already generated it!
  
  uint_t win_s = 1024;
  uint_t hop_s = win_s / 4;

  const short *samples = sb_.getSamples();
  int num_samples = sb_.getSampleCount();
  int samplerate = sb_.getSampleRate();
  int num_channels = sb_.getChannelCount();

  cout << num_samples << ", " << num_channels << endl;
  
  assert (num_samples % num_channels == 0);

  aubio_onset_t *ons[num_channels];
  aubio_tempo_t  *ts[num_channels];

  for (int i = 0; i < num_channels; i++) {
    ons[i]  = new_aubio_onset(ONSET_DETECT, win_s, hop_s, samplerate);
    ts[i]   = new_aubio_tempo(TEMPO_DETECT, win_s, hop_s, samplerate);
  }

  fvec_t * in  = new_fvec(hop_s);
  fvec_t * out = new_fvec(1);

  int num_hops = num_samples / hop_s;
  cout << num_hops << " hops left." << endl;

  int processed_hops = 0;
  int lastp = 0;

  vector<Beat> beats;

  uint step = hop_s * num_channels;
  for (uint i = 0; i < num_samples - step; i += step) {
    // Load input data_
    for (int channel = 0; channel < num_channels; channel++) {
      for (uint j = 0; j < hop_s; j++) {
	in->data[j] = samples[i + j * num_channels + channel];
      }

      // Find onsets
      aubio_onset_do(ons[channel], in, out);
      if (out->data[0]) {
	Beat b;
	b.samplenum = aubio_onset_get_last(ons[channel]);
	b.time = aubio_onset_get_last_s(ons[channel]);
	b.channel = channel;
	// TODO: pitch, intensity.
	data_.beats.push_back(b);
      }

      // Find tempos
      aubio_tempo_do(ts[channel], in, out);
      if (out->data[0]) {
	Tempo t;
	t.samplenum = aubio_tempo_get_last(ts[channel]);
	t.time = aubio_tempo_get_last_s(ts[channel]);
	t.bpm = aubio_tempo_get_bpm(ts[channel]);
	t.confidence = aubio_tempo_get_confidence(ts[channel]);
	data_.tempos.push_back(t);
      }
    
      // Progress
      int prog = 10 * (++processed_hops) / num_hops;
      if (prog > lastp) {
	lastp = prog;
	cout << prog << "0% done." << endl;
      }
    }
  }

  for (int i = 0; i < num_channels; i++) {
    del_aubio_onset(ons[i]);
    del_aubio_tempo(ts[i]);
  }
  del_fvec(in);
  del_fvec(out);

  sort(data_.beats.begin(), data_.beats.end());
  sort(data_.tempos.begin(), data_.tempos.end());

  return data_;
}
