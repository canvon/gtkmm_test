CXX      := g++
CXXFLAGS := $(shell pkg-config gtkmm-3.0 --cflags) -Wall -O2
LDFLAGS  := $(shell pkg-config gtkmm-3.0 --libs)

SRCS := $(wildcard *.cc) $(wildcard helloworld/*.cc)
OBJS := $(SRCS:.cc=.o)
BINS := simple helloworld/helloworld

all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS)

# Override GNU make implicit rule to link in a C++ way instead.
%: %.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

simple: simple.o
helloworld/helloworld: helloworld/main.o helloworld/helloworld.o
