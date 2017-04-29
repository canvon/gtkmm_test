#ifndef VERSIONCHECK_HH
#define VERSIONCHECK_HH

#if defined(STRINGIZE) || defined(STRINGIZE2)
#warning Re-defining macros STRINGIZE and/or STRINGIZE2...
#endif
#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)

// Have the gtkmm version against which this was compiled
// ready to be used as string literal.
#ifndef GTKMM_VERSION_STRING
#define GTKMM_VERSION_STRING \
	STRINGIZE2(GTKMM_MAJOR_VERSION) "." \
	STRINGIZE2(GTKMM_MINOR_VERSION) "." \
	STRINGIZE2(GTKMM_MICRO_VERSION)
#endif

// Same for glib.
#ifndef GLIB_VERSION_STRING
#define GLIB_VERSION_STRING \
	STRINGIZE2(GLIB_MAJOR_VERSION) "." \
	STRINGIZE2(GLIB_MINOR_VERSION) "." \
	STRINGIZE2(GLIB_MICRO_VERSION)
#endif

// There is a GTK_VERSION_GE(major,minor) in <gtkmm/base.h>,
// but there does not seem to be a matching GTKMM_VERSION_GE
// (as of gtkmm 3.22.0).
#ifndef GTKMM_VERSION_GE
#define GTKMM_VERSION_GE(major,minor) ((GTKMM_MAJOR_VERSION>major)||((GTKMM_MAJOR_VERSION==major)&&(GTKMM_MINOR_VERSION>=minor)))
#endif

// Same for glib.
#ifndef GLIB_VERSION_GE
#define GLIB_VERSION_GE(major,minor) ((GLIB_MAJOR_VERSION>major)||((GLIB_MAJOR_VERSION==major)&&(GLIB_MINOR_VERSION>=minor)))
#endif


// Feature checks
#if GLIB_VERSION_GE(2,50)
#define HAVE_STRUCTURED_LOGGING
#else
#define REQUIRES_STRUCTURED_LOGGING "requires glib 2.50.0 for structured logging but compiling against " GLIB_VERSION_STRING
#endif


#endif  // VERSIONCHECK_HH
