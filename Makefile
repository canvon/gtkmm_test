# Require bash to shell-script without need for a temporary file.
SHELL = /bin/bash

CXX      := g++
CXXFLAGS := $(shell pkg-config gtkmm-3.0 --cflags) -std=c++11 -Wall -O2
LDFLAGS  := $(shell pkg-config gtkmm-3.0 --libs)

# Let GNU make implicit rule link in a C++ way.
LINK.o = $(LINK.cc)

SRCS := $(wildcard *.cc) \
        $(wildcard helloworld/*.cc) \
        $(wildcard radiobuttons/*.cc) \
        $(wildcard entrycvn/*.cc)
OBJS := $(SRCS:.cc=.o)
DEPS := $(OBJS:.o=.deps)
BINS := simple helloworld/helloworld radiobuttons/radiobuttons \
        entrycvn/entrycvn

all: $(BINS)

clean:
	rm -f $(BINS) $(OBJS) $(DEPS)

simple: simple.o
helloworld/helloworld: helloworld/main.o helloworld/helloworld.o
radiobuttons/radiobuttons: radiobuttons/main.o radiobuttons/radiobuttons.o
entrycvn/entrycvn: entrycvn/main.o entrycvn/entrycvn.o

%.deps: %.cc
	set -o pipefail && \
	  OUT=$$($(COMPILE.cc) -MM $< | \
	         if [ "$(@D)" = . ]; \
	         then sed -e 's,^\([^: ]*\)\.o *:,\1.o \1.deps:,'; \
	         else sed -e 's,^\([^: ]*\)\.o *:,$(@D)/\1.o $(@D)/\1.deps:,'; \
	         fi) && \
	  [ -n "$$OUT" ] && \
	  grep -q -F "$@" <<<"$$OUT" && \
	  cat <<<"$$OUT" >$@

include $(DEPS)
