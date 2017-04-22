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

    * Sort.

    * Possibility to turn auto-completion off.

    * Respect state of Show Hidden in auto-completion, too?

    * Give annotations what type of directory entry this is?

      * E.g., append '/' for directory? (Can be seen from readlink() result.)
        Or, rather:

      * Make the completion TreeView(?) look like the dirlisting?
        With all information readily available? That would need lstat()
        on each directory entry... Performance loss?
        If this should get implemented, it must be configurable!
        It must be possible to turn this off!

  * Move directory processing to a background thread.

  * Make directory processing background thread cancellable via the GUI.

  * Add UI for (partially/fully?) canonicalize location.

  * Allow toggling symlink nofollow, to be able to see the symlink target's stats?
    Or can this be done in another way?

  * Compatibility to Debian 8 'jessie' (Debian stable as of 2017-04-19):

    > set_propagate_natural_width

    Requires gtkmm 3.22 -- can this be tested from the preprocessor?

  * Streamline toolbar creation to create a single error. (?)
    At least split the menu bar & tool bar generation which might well succeed independently of each other.

  * Implement Open.

  * Implement Close.

