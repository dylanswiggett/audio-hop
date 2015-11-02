#ifndef MUSIC_GRAPH_RENDER_
#define MUSIC_GRAPH_RENDER_

#include <vector>
#include <map>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "mg.hpp"

class MusicGraphRender {
private:
  sf::RenderWindow window_;
  float cur_t_;
  float start_t_;
  float end_t_;
  float t_width_;
  MusicGraph *graph_;
  std::vector<MusicGraphNode> nodes_;
  std::map<MusicGraphNode,float> node_positions_;
  
public:
  MusicGraphRender(MusicGraph *graph, int width, int height);
  virtual ~MusicGraphRender();
  void setTime(float t);
  void setVisible(bool visible);
  void update();
  void close();
  int isOpen();
private:
  void checkInputs();
  void render();
};

#endif // MUSIC_GRAPH_RENDER_
