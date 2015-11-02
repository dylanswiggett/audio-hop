CXX = g++
CFLAGS = -g -Wall -std=c++11
EXEC = main
SOURCES = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
LINKERS = -lsfml-audio -lsfml-window -lsfml-graphics -lsfml-system -laubio

# Main target
$(EXEC): $(OBJECTS)
	$(CXX) $(CFLAGS) $(OBJECTS) -o $(EXEC) $(LINKERS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@ $(LINKERS)

# To remove generated files
clean:
	rm -f $(EXEC) $(OBJECTS)
