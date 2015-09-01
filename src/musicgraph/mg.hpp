#ifndef MG_HPP_
#define MG_HPP_

#include <vector>

#include "../musicparse/mp.hpp"

/*
 * A single significant moment in the music.
 * This could be a beat or a note (or both).
 */
typedef struct MusicGraphNode {
  int samplenum;
  float time;
  float pitch;
  float intensity;
  float tempo; // 0 if not tempo beat, otherwise confidence.
  int bpm;
  int channel;

  // These form a DAG
  std::vector<MusicGraphNode*> children;
  std::vector<MusicGraphNode*> parents;
} MusicGraphNode;

class MusicGraph {
private:
  MusicGraphNode *root_;
public:
  MusicGraph();
  MusicGraph(AudioMetadata *metadata);
  ~MusicGraph();
  virtual void fromAudioMetadata(AudioMetadata *metadata);
private:
  void deleteGraph();
};

#endif // MG_HPP_
