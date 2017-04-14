#ifndef LS_STAT_HH
#define LS_STAT_HH

#include <memory>
#include <string>

class LsStat
{
public:
	LsStat(const char *pathname);

	bool get_is_reg(),
	     get_is_dir(),
	     get_is_chr(),
	     get_is_blk(),
	     get_is_fifo(),
	     get_is_lnk(),
	     get_is_sock();
	bool get_filemode_set_uid(),
	     get_filemode_set_gid(),
	     get_filemode_sticky(),
	     get_filemode_r_user(),
	     get_filemode_w_user(),
	     get_filemode_x_user(),
	     get_filemode_r_group(),
	     get_filemode_w_group(),
	     get_filemode_x_group(),
	     get_filemode_r_other(),
	     get_filemode_w_other(),
	     get_filemode_x_other();
	int get_filemode_rwx_user(),
	    get_filemode_rwx_group(),
	    get_filemode_rwx_other();
	std::string get_mode_str();

	int get_nlink();

	int          get_uid(),  get_gid();
	std::string  get_user(), get_group();

	long long get_size();

	// TODO: Give access to mtime, ctime, atime.

	struct stat &get_stat();

private:
	class impl;
	std::shared_ptr<impl> pimpl;
};

// TODO: Add LsLstat, which retrieves information about the symbolic link
//       instead of the symlink target.

#endif  // LS_STAT_HH
