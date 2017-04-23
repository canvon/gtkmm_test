# Require bash to shell-script without need for a temporary file.
SHELL = /bin/bash

CXX      := g++
PCCFLAGS := $(shell pkg-config gtkmm-3.0 --cflags)
CXXFLAGS := $(PCCFLAGS) -std=c++14 -Wall -O2
CFLAGS   := $(PCCFLAGS)            -Wall -O2
LDFLAGS  := $(shell pkg-config gtkmm-3.0 --libs)

# Let GNU make implicit rule link in a C++ way.
LINK.o = $(LINK.cc)

SRCS := $(wildcard *.cc) \
        $(wildcard helloworld/*.cc) \
        $(wildcard radiobuttons/*.cc) \
        $(wildcard entrycvn/*.cc) \
        $(wildcard ls-gui-cvn/*.cc)
OBJS := $(SRCS:.cc=.o) ls-gui-cvn/resources.o
DEPS := $(OBJS:.o=.deps)
BINS := simple helloworld/helloworld radiobuttons/radiobuttons \
        entrycvn/entrycvn ls-gui-cvn/ls-gui-cvn
BINS_EXTRA := ls-gui-cvn/main ls-gui-cvn/resources.c

all: $(BINS)

clean:
	rm -f $(BINS) $(BINS_EXTRA) $(OBJS) $(DEPS)

simple: simple.o
helloworld/helloworld: helloworld/main.o helloworld/helloworld.o
radiobuttons/radiobuttons: radiobuttons/main.o radiobuttons/radiobuttons.o
entrycvn/entrycvn: entrycvn/main.o entrycvn/entrycvn.o

ls-gui-cvn/ls-gui-cvn: ls-gui-cvn/main
	cp $< $@
ls-gui-cvn/main: ls-gui-cvn/main.o ls-gui-cvn/lsgui.o ls-gui-cvn/stat-cvn.o \
                 ls-gui-cvn/dirent-cvn.o ls-gui-cvn/util.o \
                 ls-gui-cvn/resources.o
ls-gui-cvn/resources.o: ls-gui-cvn/resources.c
ls-gui-cvn/resources.c: ls-gui-cvn/toolbar.gresource.xml
	#cd "$(dir $<)" && \
	#  glib-compile-resources \
	#    --target="$(notdir $@)" \
	#    --generate-source "$(notdir $<)"
	glib-compile-resources --sourcedir="$(dir $<)" \
	  --target="$@" --generate-source "$<"
ls-gui-cvn/resources.deps: ls-gui-cvn/toolbar.gresource.xml
	echo "$(@:.deps=.c) $@: $$( \
	  glib-compile-resources --sourcedir="$(dir $<)" \
	  --generate-dependencies "$<" )" >"$@"

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

# Use "-include" instead of "include", to ignore errors due to the
# dependency makefiles. Before this change, you might have needed
# to run "make -k clean" (-k as in "keep going") to get things going again.
-include $(DEPS)
