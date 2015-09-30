#ifndef MG_HPP_
#define MG_HPP_

#include <vector>

#include "../musicparse/mp.hpp"

/*
 * A single significant moment in the music.
 * This could be a beat or a note (or both).
 */
class MusicGraphNode {
public:
  int samplenum;
  float time;
  float pitch; // 0 (beat) or >0 (note)
  float intensity;
  float tempo; // 0 (note) or tempo (beat).
  float tempoconfidence;
  std::set<int> channels;

  // These form a DAG
  std::vector<MusicGraphNode*> children;
  MusicGraphNode *parent;

  int isbeat() { return tempo; }
  int isnote() { return pitch; }

  virtual ~MusicGraphNode() { }

  virtual int operator<(MusicGraphNode &o) { return time < o.time; }
private:
  std::vector<float> getFeatureVector();
public:
  float weightAsChild(MusicGraphNode *n);
};

class MusicGraph {
private:
  MusicGraphNode *root_;
  std::set<MusicGraphNode*> leaves_;
public:
  MusicGraph();
  MusicGraph(AudioMetadata *metadata);
  ~MusicGraph();
  virtual void fromAudioMetadata(AudioMetadata *metadata);
  virtual std::vector<MusicGraphNode> flatten();
  virtual MusicGraphNode* root();
private:
  void addNode(MusicGraphNode *node);
  void deleteGraph();
};

#endif // MG_HPP_
