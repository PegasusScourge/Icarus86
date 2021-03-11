rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

IDIR := include
LIDIR := lib_include
SDIR := src
LDIR := lib
BINDIR := bin

BIN_NAME := Icarus86.exe

CXX := g++ -std=c++17
CXXFLAGS := -I$(IDIR) -I$(LIDIR) -D SFML_STATIC -m64 -Wall
LFLAGS := -L$(LDIR) -lsfml-graphics-s -lopengl32 -lfreetype -lsfml-system-s -lwinmm -lsfml-window-s -lgdi32

CXX_SRCS := $(call rwildcard,$(SDIR),*.cpp)

CXX_HDRS := $(call rwildcard,$(IDIR),*.hpp)

CXX_OBJS := $(patsubst %.cpp,%.o,$(CXX_SRCS))

all: $(CXX_HDRS) $(BINDIR)/$(BIN_NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(LFLAGS) -c -o $@ $^

$(BINDIR)/$(BIN_NAME): $(CXX_OBJS)
	$(CXX) $(CXXFLAGS) $(LFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(CXX_OBJS)
	rm -f $(BINDIR)/$(BIN_NAME)