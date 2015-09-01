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

  AudioMetadata metadata = a.getMetadata();
  cout << "Found " << metadata.beats.size() << " beats." << endl;

  a.play();

  for (auto b : metadata.tempos) {
    float wait = b.time - a.curTime();
    if (wait > 0) // Maybe not the case w/ multiple channels
      usleep(1000000 * wait);
    cout << b.channel << flush;
  }

  MusicGraph g(&metadata);

  return 0;
}
