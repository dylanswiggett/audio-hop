#include "mg.hpp"

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
}

void MusicGraph::deleteGraph()
{
  if (root_ == nullptr)
    return;
  // Delete graph
}
