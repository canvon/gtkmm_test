# canvon/gtkmm\_test - TODO list

## ls-gui-cvn

  * Complete the symlink support -- but what would still need to get implemented?

  * Implement date/time processing & display.

  * Implement uid/gid -> user/group lookup.

  * Right-align more numbers: uid/gid

  * Replace ListStore by TreeStore and allow manual descend into child directories.

  * Allow sorting: Allow switching back to unsorted.

  * Have auto-completion in the location entry:
    Test, test, test and adjust for corner cases.

  * Move directory processing to a background thread.

  * Make directory processing background thread cancellable via the GUI.

  * Add UI for (partially/fully?) canonicalize location.

  * Allow toggling symlink nofollow, to be able to see the symlink target's stats?
    Or can this be done in another way?

