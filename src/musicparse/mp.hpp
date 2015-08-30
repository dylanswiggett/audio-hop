#ifndef MP_HPP_
#define MP_HPP_

#include <vector>

#include <SFML/Audio.hpp>

typedef struct Beat {
  int samplenum;
  float time;
  float intensity;
  int channel;
} Beat;

typedef struct AudioMetadata {
  std::vector<Beat> beats;
} AudioMetadata;

class Audio {
private:
  sf::SoundBuffer sb_;
  sf::Sound sound_;
  bool ready_;
public:
  Audio(const char *path);
  virtual ~Audio();
  virtual bool ready() { return ready_; };
  virtual void play();
  virtual float curTime();
  std::vector<Beat> getBeats();
};

#endif // MP_HPP_
