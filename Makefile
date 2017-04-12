CXX      := g++
CXXFLAGS := $(shell pkg-config gtkmm-3.0 --cflags) -Wall -O2
LDFLAGS  := $(shell pkg-config gtkmm-3.0 --libs)

# Let GNU make implicit rule link in a C++ way.
LINK.o = $(LINK.cc)

SRCS := $(wildcard *.cc) $(wildcard helloworld/*.cc)
OBJS := $(SRCS:.cc=.o)
BINS := simple helloworld/helloworld

all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS)

simple: simple.o
helloworld/helloworld: helloworld/main.o helloworld/helloworld.o
