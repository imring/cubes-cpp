CC=g++
CFLAGS=-c -Wall
CLIBS=-lsfml-system -lsfml-window -lsfml-graphics -lsfml-network

SOURCES=main.cpp menu.cpp color.cpp player.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=cubes

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(OBJECTS) $(CLIBS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)