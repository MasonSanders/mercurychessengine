CXX := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -O3 -DNDEBUG -pthread

TARGET := mercury
SRCS := src/main.cpp src/Board.cpp src/Engine.cpp src/TranspositionTable.cpp
OBJDIR := bin
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

HEADERS := include/Board.h include/Piece.h include/Move.h include/MoveType.h include/BitboardUtils.h include/Engine.h include/TranspositionTable.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all run clean
