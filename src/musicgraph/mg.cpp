#include <cassert>
#include <cmath>
#include <vector>
#include <set>
#include <iostream>

#include "mg.hpp"

using namespace std;

#define MAX_PARENT_DELAY 5.0
#define MAX_PARENT_DEPTH 4

vector<float> MusicGraphNode::getFeatureVector()
{
  vector<float> features;
  // TODO Weight features? Adaptively?
  features.push_back(time);
  features.push_back(pitch);
  features.push_back(intensity);
  return features;
}

float MusicGraphNode::weightAsChild(MusicGraphNode *n)
{
  float similarity = 0;

  vector<float> features = getFeatureVector();
  vector<float> otherFeatures = n->getFeatureVector();

  assert(features.size() == otherFeatures.size());

  for (unsigned int i = 0; i < features.size(); i++) {
    float diff = features[i] - otherFeatures[i];
    similarity += diff * diff;
  }
  
  return -sqrt(similarity); // Closer to zero = better
}

MusicGraph::MusicGraph()
{
  root_ = nullptr;
}


MusicGraph::MusicGraph(AudioMetadata *metadata)
{
  root_ = nullptr;
  fromAudioMetadata(metadata);
}

MusicGraph::~MusicGraph()
{
  deleteGraph();
}

void MusicGraph::fromAudioMetadata(AudioMetadata *metadata)
{
  deleteGraph();
  
  // Generate graph
  MusicGraphNode *n = new MusicGraphNode();

  vector<Beat> beats(metadata->beats);
  vector<Tempo> tempos(metadata->tempos);

  float curtempo = .1;
  //float curtempo_confidence = 0; // TODO

  while (beats.size() || tempos.size()) {
    float maxbeatwidth = curtempo / 32;
    
    n->time = 1000000;
    n->samplenum = 100000000;
    if (beats.size()) {
      n->time = beats[0].time;
      n->samplenum = beats[0].samplenum;
    }
    if (tempos.size()) {
      n->time = min(n->time, tempos[0].time);
      n->samplenum = min(n->samplenum, tempos[0].samplenum);
    }

    cout << "beat/note at " << n->time << " s" << endl;

    // Now we find all notes close enough together to be on this beat.
    while (beats.size() > 0 && beats[0].time - n->time < maxbeatwidth) {
      Beat beat = beats[0];
      beats.erase(beats.begin());

      // TODO: Averaging?
      n->pitch = beat.pitch;
      n->intensity = beat.intensity;
      n->channels.insert(beat.channel);
    }

    while (tempos.size() > 0 && tempos[0].time - n->time < maxbeatwidth) {
      // TODO: Get tempo more intelligently.
      Tempo tempo = tempos[0];
      tempos.erase(tempos.begin());

      n->tempo = tempo.bpm;
      n->tempoconfidence = tempo.confidence;
      n->channels.insert(tempo.channel);
    }
    
    addNode(n);
    n = new MusicGraphNode();
  }
  delete n;
}

vector<MusicGraphNode> MusicGraph::flatten()
{
  set<MusicGraphNode*> nodes;

  for (auto leaf : leaves_) {
    while (leaf && !nodes.count(leaf)) {
      nodes.insert(leaf);
      leaf = leaf->parent;
    }
  }

  vector<MusicGraphNode> sortednodes;
  for (auto n : nodes) sortednodes.push_back(*n);
  sort(sortednodes.begin(), sortednodes.end());

  vector<MusicGraphNode> out;
  for (auto n : sortednodes) {
    //n.parent = nullptr;
    //n.children.clear();
    out.push_back(n);
  }

  return out;
}

MusicGraphNode* MusicGraph::root()
{
  return root_;
}

void MusicGraph::addNode(MusicGraphNode *node)
{
  if (root_ == nullptr) {
    root_ = node;
    leaves_.insert(root_);
    return;
  }

  set<MusicGraphNode*> prevnodes;

  // Find all parent candidates
  for (auto leaf : leaves_) {
    for (int depth = 0; leaf && depth < MAX_PARENT_DEPTH &&
	   node->time - leaf->time < MAX_PARENT_DELAY &&
	   !prevnodes.count(leaf); depth++) {
      prevnodes.insert(leaf);
      leaf = leaf->parent;
    }
  };

  MusicGraphNode *bestparent;
  float bestparentweight = -100000;

  for (auto prev : prevnodes) {
    float weight = prev->weightAsChild(node);
    if (weight > bestparentweight) {
      bestparentweight = weight;
      bestparent = prev;
    }
  };

  bestparent->children.push_back(node);
  node->parent = bestparent;
  leaves_.erase(bestparent);
  leaves_.insert(node);
}

void MusicGraph::deleteGraph()
{
  if (root_ == nullptr)
    return;
  // Delete graph
}
