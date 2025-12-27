CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I../../engine/include

ENGINE_BUILD = ../../engine/build
ENGINE_OBJS = $(ENGINE_BUILD)/board.o $(ENGINE_BUILD)/move.o $(ENGINE_BUILD)/scorer.o

TARGET = score_15_letter_words
SRC = score_15_letter_words.cpp

all: $(TARGET)

$(TARGET): $(SRC) $(ENGINE_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(ENGINE_OBJS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
