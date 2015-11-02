#include <unistd.h>
#include <iostream>
#include <vector>

#include "musicparse/mp.hpp"
#include "musicgraph/mg.hpp"
#include "musicgraph/MusicGraphRender.hpp"

using namespace std;

int main(int argc, char **argv)
{
  if (argc != 2) {
    cout << "usage: ./main <audio path>" << endl;
    return 1;
  }

  Audio a(argv[1]);
  if (!a.ready())
    return 1;

  AudioMetadata meta = a.getMetadata();
  MusicGraph g(&meta);

  vector<MusicGraphNode> ns = g.flatten();

  cout << "Found " << ns.size() << " beats." << endl;

  MusicGraphRender render(&g, 800, 600);
  render.setTime(0);
  render.setVisible(true);
  a.play();

  while (render.isOpen()) {
    render.setTime(a.curTime());
    render.update();
  }
  
  /*
  a.play();

  vector<MusicGraphNode*> currentNodes;
  vector<MusicGraphNode*> newNodes;
  currentNodes.push_back(g.root());

  while (currentNodes.size() > 0) {
    newNodes.clear();
    
    float earliest = 1000000;

    for (auto node : currentNodes) {
      if (node->time <= a.curTime()) {
	if (node->isbeat())
	  cout << "X ";
	else
	  cout << "* ";
	for (auto child : node->children) {
	  newNodes.push_back(child);
	  earliest = min(child->time, earliest);
	}
      } else {
	cout << "  ";
	newNodes.push_back(node);
	earliest = min(node->time, earliest);
      }
    }

    cout << endl;
    currentNodes = newNodes;
    float wait = earliest - a.curTime();
    if (wait > 0)
      usleep(1000000 * wait);
  }

  /* Flattened printout
  for (auto n : ns) {
    float wait = n.time - a.curTime();
    if (wait > 0) // Maybe not the case w/ multiple channels
      usleep(1000000 * wait);
    if (n.isbeat())
      cout << '1' << flush;
    else
      cout << '0' << flush;
  }
  */

  return 0;
}
