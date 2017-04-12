CXX      := g++
CXXFLAGS := $(shell pkg-config gtkmm-3.0 --cflags) -Wall -O2
LDFLAGS  := $(shell pkg-config gtkmm-3.0 --libs)

#SRCS := $(wildcard *.cc)
#OBJS := $(SRCS:.cc=.o)
BINS := simple

all: $(BINS)

clean:
#	rm -f $(BINS) $(OBJS)
	rm -f $(BINS)

#simple: simple.o
