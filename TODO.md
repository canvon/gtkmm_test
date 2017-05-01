# canvon/gtkmm\_test - TODO list

## ls-gui-cvn

  * Complete the symlink support -- but what would still need to get implemented?

  * Implement date/time processing & display.

  * Allow switching back to numerical uid/gid display.

  * Right-align more numbers: uid/gid

    * Also sort numerically when these fields are displayed as numbers.

  * Replace ListStore by TreeStore and allow manual descend into child directories.

  * Allow sorting: Allow switching back to unsorted.

  * Have auto-completion in the location entry:

    * Test, test, test and adjust for corner cases.

    * Give annotations what type of directory entry this is?

      * E.g., append '/' for directory? (Can be seen from readlink() result.)
        Or, rather:

      * Make the completion TreeView(?) look like the dirlisting?
        With all information readily available? That would need lstat()
        on each directory entry... Performance loss?
        If this should get implemented, it must be configurable!
        It must be possible to turn this off!

    * Update when backspacing over a slash '/'.
      (Or rather, when the directory part of the typed location changes?)

  * Move directory processing to a background thread.

  * Make directory processing background thread cancellable via the GUI.

  * Add UI for (partially/fully?) canonicalize location.

  * Allow toggling symlink nofollow, to be able to see the symlink target's stats?
    Or can this be done in another way?

  * Use more try-catch where things might fail, e.g. at charset conversion
    (like in Glib::filename_to_utf8()).

  * Have a Home button, for easily opening $HOME?
    Have a Current working directory button, for easily opening "."?
    Or is both sufficiently easily to achieve by using the file open dialog?

  * Support ~/ and ~USERNAME tilde expansion?
    But what about $HOME vs. getpwnam() inconsistency?
    For our user, $HOME should be used, but should ~/ and ~OURUSER/ potentially differ?
    Maybe have a look at what exactly bash (or some other shell) does?

  * Support environment variable expansion, e.g., on Ctrl-Enter?
    (Or maybe according to toggle button?)

  * Let file open dialog be a member? It would continue to live, then,
    potentially giving the user the last used state as starting point
    for further opens. (This could be a problem when introducing tabs, though.)

  * Use inotify/dnotify, to (optionally?) automatically update the displayed data
    when it changes on-disk? (Need to be able to disable this!)

  * Change location history to track the opsys path (in filename encoding),
    as an std::string, instead of tracking the user-visible Glib::ustring's?
    Could have pros/cons either way! The idea for a change being that
    the GUI can access filesystem objects by their "real" name and as such
    better cope with reencoding errors, which would be display-only, then.

  * Use pimpl (pointer to implementation) technique with gtkmm-based classes,
    too? Hide the details, have less inter-dependency between modules,
    require less recompilation on an internal-only change.

