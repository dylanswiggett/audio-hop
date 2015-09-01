#ifndef MP_HPP_
#define MP_HPP_

#include <vector>

#include <SFML/Audio.hpp>

typedef struct Beat {
  int samplenum;
  float time;
  float pitch; // Unused
  float intensity; // Unused
  int channel;

  virtual int operator<(Beat &o) { return time < o.time; }
} Beat;

typedef struct Tempo {
  int samplenum;
  float time;
  float bpm;
  float confidence;
  int channel;

  virtual int operator<(Tempo &o) { return time < o.time; }
} Tempo;

typedef struct AudioMetadata {
  std::vector<Beat> beats;
  std::vector<Tempo> tempos;
} AudioMetadata;

class Audio {
private:
  sf::SoundBuffer sb_;
  sf::Sound sound_;
  AudioMetadata data_;
  bool ready_;
public:
  Audio(const char *path);
  virtual ~Audio();
  virtual bool ready() { return ready_; };
  virtual void play();
  virtual float curTime();
  AudioMetadata getMetadata();
};

#endif // MP_HPP_
