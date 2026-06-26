CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -O3 -DNDEBUG -pthread

TARGET := mercury
SRCS := main.cpp Board.cpp Engine.cpp TranspositionTable.cpp
OBJS := $(SRCS:.cpp=.o)

HEADERS := Board.h Piece.h Move.h MoveType.h BitboardUtils.h Engine.h TranspositionTable.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
