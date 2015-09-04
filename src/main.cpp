#include <unistd.h>
#include <iostream>
#include <vector>

#include "musicparse/mp.hpp"
#include "musicgraph/mg.hpp"

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

  a.play();

  for (auto n : ns) {
    float wait = n.time - a.curTime();
    if (wait > 0) // Maybe not the case w/ multiple channels
      usleep(1000000 * wait);
    cout << '0' << flush;
  }

  return 0;
}
