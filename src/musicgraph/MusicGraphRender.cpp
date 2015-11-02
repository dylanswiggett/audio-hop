#include <iostream>
using namespace std;

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "MusicGraphRender.hpp"

#define WINDOW_TITLE "Music Graph Render"

#define WINDOW_TIME 10 // Seconds
#define STD_TEMPO_LOC 0.5

#define TEMPO_SIZE 2

struct RenderGraphNode {
public:
  MusicGraphNode n;
  float xpos;
  float ypos;
  float size;
  sf::Color color;
};

MusicGraphRender::MusicGraphRender(MusicGraph *graph, int width, int height) :
  window_(sf::VideoMode(width, height), WINDOW_TITLE)
{
  graph_ = graph;
  nodes_ = graph->flatten();
  setTime(0);

  int npos = 5;
  int curpos = 1;
  int posh = height / (npos + 1);

  // TODO: Generate internal graph representation.
  
  for (MusicGraphNode node : nodes_) {
    node_positions_[node] = posh * curpos;
    
    if (++curpos > npos)
      curpos = 1;
  }
}

MusicGraphRender::~MusicGraphRender()
{
  if (isOpen())
    close();
}

void MusicGraphRender::setTime(float t)
{
  cur_t_ = t;
  sf::Vector2u wsize = window_.getSize();
  start_t_ = cur_t_ - STD_TEMPO_LOC * WINDOW_TIME;
  end_t_ = start_t_ + WINDOW_TIME;
  
  // TODO: Adjust for end_t_ past the end of the song.
  if (start_t_ < 0) {
    start_t_ = 0;
    end_t_ = WINDOW_TIME;
  }

  t_width_ = ((float)wsize.x) / WINDOW_TIME; // px/sec
}

void MusicGraphRender::setVisible(bool visible)
{
  window_.setVisible(visible);
}

void MusicGraphRender::close()
{
  window_.close();
}

int MusicGraphRender::isOpen()
{
  return window_.isOpen();
}

void MusicGraphRender::update()
{
  checkInputs();
  render();
}

void MusicGraphRender::checkInputs()
{
  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Closed)
      close();
    
    // TODO: Use more events?
  }
}

void MusicGraphRender::render()
{
  sf::Vector2u wsize = window_.getSize();
  int window_h = wsize.y;
  
  window_.clear(sf::Color::White);

  // TODO: Make selection of nodes more efficient (and correct).
  for (MusicGraphNode node : nodes_) {
    // TODO: This doesn't take the rendered width of nodes into account.
    if (node.time < start_t_ || node.time > end_t_)
      continue;

    if (node.isbeat()) {
      sf::RectangleShape node_shape(sf::Vector2f(TEMPO_SIZE, window_h));
      node_shape.setPosition(t_width_ * (node.time - start_t_), 0);
      node_shape.setFillColor(sf::Color::Green);
      window_.draw(node_shape);
    }

    if (node.isnote()) {
      int w = node.intensity / 150;
      sf::CircleShape node_shape(w);
      node_shape.setPosition(t_width_ * (node.time - start_t_) - w/2, node_positions_[node]);
      node_shape.setFillColor(sf::Color::Red);
      window_.draw(node_shape);

      if (node.parent != nullptr){
	MusicGraphNode n = *(node.parent);
	sf::Vertex line[] = {
	  sf::Vertex(sf::Vector2f(t_width_ * (n.time - start_t_),
				  node_positions_[n] + w/2), sf::Color::Red),
	  sf::Vertex(sf::Vector2f(t_width_ * (node.time - start_t_),
				  node_positions_[node] + w/2), sf::Color::Red)
	};
	window_.draw(line, 2, sf::Lines);
      }

      for (MusicGraphNode *np : node.children) {
	MusicGraphNode n = *np;
	sf::Vertex line[] = {
	  sf::Vertex(sf::Vector2f(t_width_ * (n.time - start_t_),
				  node_positions_[n] + w/2), sf::Color::Red),
	  sf::Vertex(sf::Vector2f(t_width_ * (node.time - start_t_),
				  node_positions_[node] + w/2), sf::Color::Red)
	};
	window_.draw(line, 2, sf::Lines);

      }
    }
  }

  // Rending current position.
  sf::RectangleShape time_marker(sf::Vector2f(TEMPO_SIZE, window_h));
  time_marker.setPosition(t_width_ * (cur_t_ - start_t_), 0);
  time_marker.setFillColor(sf::Color::Black);
  window_.draw(time_marker);

  window_.display();
}
