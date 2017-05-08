# canvon/gtkmm\_test


## ToC

Table of Contents:

  * [Introduction to gtkmm_test/ls-gui-cvn](#introduction)

  * [How to build from the code base](#build)

  * [How to run the created application(s)](#run)

  * [Development, or programming on this](#develop)

  * [Origin of the software or where does the code come from](#origin)


## Introduction

This is a test for using the gtkmm C++ binding for the GTK toolkit.
That is, this is about creating GUIs (Graphical User Interfaces)
and learning C++ (again).

This also contains ls-gui-cvn, an ls GUI by [canvon][]. Much work
went into this, hopefully the time was not completely wasted.

[canvon]: https://github.com/canvon "canvon on GitHub"

As of 2017-05-01, this code base uses a custom, hand-written makefile
that does the job well but is not expected to work on anything other
than (Debian) GNU/Linux(, yet?), as it uses bash features in some
of the make recipes and possibly uses GNU make (a.k.a. "gmake") features
throughout the Makefile.  It is expected that, some day, the code base
will be switched over to using CMake instead, which promises to be
more portable.


## Build

You'll need a C++ compiler with C++14 support, and the gtkmm (and related)
development packages, discoverable via pkg-config. Furthermore you'll need
GNU bash, the Bourne Again SHell, and GNU make (on some platforms known/
installed as "gmake").

On Debian 8 or later, use the following command line (or similar), as root
(or using sudo), to automatically download and install the needed dependencies:

	# apt-get install build-essential libgtkmm-3.0-dev

To build the binaries, issue (as user, in the directory of the code base):

	gtkmm_test$ make all

This is also the default target, so you could simply use:

	gtkmm_test$ make

If you're only interested in the ls-gui-cvn, instead use:

	gtkmm_test$ make ls-gui-cvn/ls-gui-cvn


## Run

To use the built application(s), start them from the terminal like this:

	gtkmm_test$ ./ls-gui-cvn/ls-gui-cvn

If you should have non-UTF-8 path name encodings, use this (for latin1):

	gtkmm_test$ G_FILENAME_ENCODING=ISO-8859-1 ./ls-gui-cvn/ls-gui-cvn


## Develop

This project started out being edited in VIM (vi improved, text editor
for programmers), with the distribution-installed book "Programming
with gtkmm 3" open in the web browser as reference. Later, Devhelp
was always open to look up the API reference for gtkmm or sometimes
GTK and glib.

After some search for a free (as in speech) C++ code completion tool,
the CodeLite IDE (Integrated Development Environment) was chosen.
It is currently used to edit this code base and associated files
when possible, though some edits in relation with the git source code
management system are still done in VIM, some of the time.

Also, porting to Debian 8 'jessie' (as of 2017-05-01, Debian stable)
is made using VIM in an edit-compile-run cycle in a systemd-nspawn
container. The host (which I do usual development on) is running
upcoming Debian 9 'stretch' (as of 2017-05-01, Debian testing).

If you should have tried (or even succeeded in) building and/or running this
on something non-GNU/Linux (e.g., some UNIX, MacOS X or even Windows),
I'd be happy to hear about it. Maybe the source and/or build system
can be adapted so it'll work out-of-the-box on that platform, soon.


## Origin

The initially checked-in material was from the online book ["Programming
with gtkmm 3"][book-gtkmm3]. This includes the projects `simple`, `helloworld`,
and `radiobuttons`. They appear to be [under GPL-2][examples-license].
The `entrycvn` was coded up after looking at a screenshot of another example,
but not looking at the provided source code.

[book-gtkmm3]: https://developer.gnome.org/gtkmm-tutorial/stable/
[examples-license]:
  https://git.gnome.org/browse/gtkmm-documentation/tree/COPYING.examples?h=gtkmm-3-22

The `ls-gui-cvn`, however, was devised and implemented by the initial author
of this README, Fabian Pietsch a.k.a. `canvon`.

The hand-written Makefile is also by canvon.

Attribution & copyright claim headers are currently missing from the
source files as of 2017-05-01. Also, there is no licensing, yet.
