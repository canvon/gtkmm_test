# canvon/gtkmm\_test - TODO list


## Common

  * Add attribution & copyright claim headers to all source files.

  * License, as possible.  Find out what the original source code
    from "Programming with gtkmm 3" may be licensed as. (Can be
	different than the book's license, or could be dual-licensed
	GFDL + GPL or something.)

  * Switch to CMake?


## ls-gui-cvn

  * Complete the symlink support -- but what would still need to get implemented?

    * Perhaps keep symlink targets in the model,
      and switch to them using set_location_str_relative()
      if the location already is equal to the symlink's pathname.

  * Implement date/time processing & display.

  * Allow switching back to numerical uid/gid display.

  * Right-align more numbers: uid/gid

    * Also sort numerically when these fields are displayed as numbers.

  * Replace ListStore by TreeStore and allow manual descend into child directories.

  * Allow sorting: Allow switching back to unsorted.

  * Allow sorting by time column and later, which currently breaks
    due to a difference between view column number and model column number...

  * Allow sorting directories first.

  * Have auto-completion in the location entry:

    * Test, test, test and adjust for corner cases.

  * Move directory processing to a background thread.

  * Make directory processing background thread cancellable via the GUI.

  * Add UI for (partially/fully?) canonicalize location.

  * Allow toggling symlink nofollow, to be able to see the symlink target's stats?
    Or can this be done in another way?

  * Use more try-catch where things might fail, e.g. at charset conversion
    (like in Glib::filename_to_utf8()).

  * Support environment variable expansion, e.g., on Ctrl-Enter?
    (Or maybe according to toggle button?)

  * Let file open dialog be a member? It would continue to live, then,
    potentially giving the user the last used state as starting point
    for further opens. (This could be a problem when introducing tabs, though.)

  * Let the user provide fallback charset(s?) to try conversion with when
    the one set by G_FILENAME_ENCODING environment variable gives conversion
    error.

  * Let the user configure a list of pathname prefixes and what encoding
    to use for them.

  * Use inotify/dnotify, to (optionally?) automatically update the displayed data
    when it changes on-disk? (Need to be able to disable this!)

  * Use pimpl (pointer to implementation) technique with gtkmm-based classes,
    too? Hide the details, have less inter-dependency between modules,
    require less recompilation on an internal-only change.

  * Add an Application/Desktop file to be placed in, e.g.,
    ~/.local/share/applications/ or installed system-wide.
    (Needed for making the GUI known to the GNOME Shell.)

