# canvon/gtkmm\_test - TODO list

## ls-gui-cvn

  * Complete the symlink support -- but what would still need to get implemented?

  * Implement date/time processing & display.

  * Implement uid/gid -> user/group lookup.

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

  * Implement Open.

  * Implement Close.

  * Don't add history element when current location is re-entered.
    Should stop adding more and more duplicates when pressing Enter again.

