#ifndef STAT_CVN_HH
#define STAT_CVN_HH

#include <memory>
#include <string>

// A stat() and struct stat wrapper.
class LsStat
{
public:
	LsStat(const char *pathname);
	LsStat(const std::string &pathname_str);

	bool get_is_reg () const,
	     get_is_dir () const,
	     get_is_chr () const,
	     get_is_blk () const,
	     get_is_fifo() const,
	     get_is_lnk () const,
	     get_is_sock() const;
	bool get_filemode_set_uid() const,
	     get_filemode_set_gid() const,
	     get_filemode_sticky () const,
	     get_filemode_r_user () const,
	     get_filemode_w_user () const,
	     get_filemode_x_user () const,
	     get_filemode_r_group() const,
	     get_filemode_w_group() const,
	     get_filemode_x_group() const,
	     get_filemode_r_other() const,
	     get_filemode_w_other() const,
	     get_filemode_x_other() const;
	int get_filemode_rwx_user () const,
	    get_filemode_rwx_group() const,
	    get_filemode_rwx_other() const;
	std::string get_mode_str() const;

	int get_nlink() const;

	int          get_uid () const,  get_gid  () const;
	std::string  get_user() const,  get_group() const;

	long long get_size() const;

	// TODO: Give access to mtime, ctime, atime.

	struct stat &get_stat();
	const struct stat &get_stat() const;

protected:
	LsStat();  // For use by derived classes.

	class impl;
	std::shared_ptr<impl> pimpl;
};

// LsLstat retrieves information about the symbolic link instead of
// the symlink target. (Uses lstat() internally instead of stat().)
class LsLstat : public LsStat
{
public:
	LsLstat(const char *pathname);
	LsLstat(const std::string &pathname_str);
};

// LsFstatat works from an already opened directory. (Uses fstatat().)
class LsFstatat : public LsStat
{
public:
	LsFstatat(int dirfd, const char *pathname, bool symlink_nofollow = false);
	LsFstatat(int dirfd, const std::string &pathname_str, bool symlink_nofollow = false);
	LsFstatat(const char *pathname) = delete;
	LsFstatat(const std::string &pathname_str) = delete;
};

#endif  // STAT_CVN_HH
