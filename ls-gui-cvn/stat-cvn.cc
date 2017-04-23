#include "stat-cvn.hh"
#include <system_error>
#include <iomanip>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


class LsStat::impl
{
public:
	struct stat sb;  // stat buffer
};

// Basic constructor, to be used by LsStat itself and all derived classes.
LsStat::LsStat()
{
	// Initialize smartpointer.
	pimpl = std::make_shared<LsStat::impl>();
}


// Constructors for stat().

LsStat::LsStat(const char *pathname) :
	LsStat()
{
	if (!pathname)
		throw std::invalid_argument("LsStat ctor: argument pathname is required");

	if (stat(pathname, &pimpl->sb)) {
		std::ostringstream os;
		os << "LsStat ctor: syscall stat() failed for " << std::quoted(pathname);
		throw std::system_error(errno, std::generic_category(), os.str());
	}
}

LsStat::LsStat(const std::string &pathname_str) :
	LsStat(pathname_str.c_str())
{
}


// Constructors for lstat() (instead of stat()).

LsLstat::LsLstat(const char *pathname) :
	LsStat()
{
	if (!pathname)
		throw std::invalid_argument("LsLstat ctor: argument pathname is required");

	if (lstat(pathname, &pimpl->sb)) {
		std::ostringstream os;
		os << "LsLstat ctor: syscall lstat() failed for " << std::quoted(pathname);
		throw std::system_error(errno, std::generic_category(), os.str());
	}
}

LsLstat::LsLstat(const std::string &pathname_str) :
	LsLstat(pathname_str.c_str())
{
}


// Constructors for fstatat() (instead of stat()).

LsFstatat::LsFstatat(int dirfd, const char *pathname, bool symlink_nofollow) :
	LsStat()
{
	// (Allow possible special-case.)
	//if (!pathname)
	//	throw std::invalid_argument("LsFstatat ctor: argument pathname is required");

	int flags = 0;
	if (symlink_nofollow)
		flags |= AT_SYMLINK_NOFOLLOW;
	if (fstatat(dirfd, pathname, &pimpl->sb, flags)) {
		std::ostringstream os;
		os << "LsFstatat ctor: syscall fstatat() failed at file descriptor "
		   << dirfd << " for " << std::quoted(pathname);
		if (flags)
			os << " (with flags " << flags << ")";
		throw std::system_error(errno, std::generic_category(), os.str());
	}
}

LsFstatat::LsFstatat(int dirfd, const std::string &pathname_str, bool symlink_nofollow) :
	LsFstatat(dirfd, pathname_str.c_str(), symlink_nofollow)
{
}


// The rest is shared between LsStat and all derived classes.

bool LsStat::get_is_reg () const { return S_ISREG (pimpl->sb.st_mode); }
bool LsStat::get_is_dir () const { return S_ISDIR (pimpl->sb.st_mode); }
bool LsStat::get_is_chr () const { return S_ISCHR (pimpl->sb.st_mode); }
bool LsStat::get_is_blk () const { return S_ISBLK (pimpl->sb.st_mode); }
bool LsStat::get_is_fifo() const { return S_ISFIFO(pimpl->sb.st_mode); }
bool LsStat::get_is_lnk () const { return S_ISLNK (pimpl->sb.st_mode); }
bool LsStat::get_is_sock() const { return S_ISSOCK(pimpl->sb.st_mode); }

bool LsStat::get_filemode_set_uid() const  { return (pimpl->sb.st_mode & S_ISUID) == S_ISUID; }
bool LsStat::get_filemode_set_gid() const  { return (pimpl->sb.st_mode & S_ISGID) == S_ISGID; }
bool LsStat::get_filemode_sticky () const  { return (pimpl->sb.st_mode & S_ISVTX) == S_ISVTX; }
bool LsStat::get_filemode_r_user () const  { return (pimpl->sb.st_mode & S_IRUSR) == S_IRUSR; }
bool LsStat::get_filemode_w_user () const  { return (pimpl->sb.st_mode & S_IWUSR) == S_IWUSR; }
bool LsStat::get_filemode_x_user () const  { return (pimpl->sb.st_mode & S_IXUSR) == S_IXUSR; }
bool LsStat::get_filemode_r_group() const  { return (pimpl->sb.st_mode & S_IRGRP) == S_IRGRP; }
bool LsStat::get_filemode_w_group() const  { return (pimpl->sb.st_mode & S_IWGRP) == S_IWGRP; }
bool LsStat::get_filemode_x_group() const  { return (pimpl->sb.st_mode & S_IXGRP) == S_IXGRP; }
bool LsStat::get_filemode_r_other() const  { return (pimpl->sb.st_mode & S_IROTH) == S_IROTH; }
bool LsStat::get_filemode_w_other() const  { return (pimpl->sb.st_mode & S_IWOTH) == S_IWOTH; }
bool LsStat::get_filemode_x_other() const  { return (pimpl->sb.st_mode & S_IXOTH) == S_IXOTH; }

int LsStat::get_filemode_rwx_user () const { return pimpl->sb.st_mode & S_IRWXU; }
int LsStat::get_filemode_rwx_group() const { return pimpl->sb.st_mode & S_IRWXG; }
int LsStat::get_filemode_rwx_other() const { return pimpl->sb.st_mode & S_IRWXO; }

std::string LsStat::get_mode_str() const
{
	std::ostringstream os;


	// file type

	if      (get_is_reg())   os << '-';
	else if (get_is_dir())   os << 'd';
	else if (get_is_chr())   os << 'c';
	else if (get_is_blk())   os << 'b';
	else if (get_is_fifo())  os << 'p';
	else if (get_is_lnk())   os << 'l';
	else if (get_is_sock())  os << 's';
	else                     os << '?';


	// user perms

	if      (get_filemode_r_user())  os << 'r';
	else                             os << '-';

	if      (get_filemode_w_user())  os << 'w';
	else                             os << '-';

	if (get_filemode_set_uid()) {
		if      (get_filemode_x_user())  os << 's';
		else                             os << 'S';
	}
	else {
		if      (get_filemode_x_user())  os << 'x';
		else                             os << '-';
	}


	// group perms

	if      (get_filemode_r_group())  os << 'r';
	else                              os << '-';

	if      (get_filemode_w_group())  os << 'w';
	else                              os << '-';

	if (get_filemode_set_gid()) {
		if      (get_filemode_x_group())  os << 's';
		else                              os << 'S';
	}
	else {
		if      (get_filemode_x_group())  os << 'x';
		else                              os << '-';
	}


	// other perms

	if      (get_filemode_r_other())  os << 'r';
	else                              os << '-';

	if      (get_filemode_w_other())  os << 'w';
	else                              os << '-';

	if (get_filemode_sticky()) {
		if      (get_filemode_x_other())  os << 't';
		else                              os << 'T';
	}
	else {
		if      (get_filemode_x_other())  os << 'x';
		else                              os << '-';
	}


	return os.str();
}

int LsStat::get_nlink() const { return pimpl->sb.st_nlink; }

int LsStat::get_uid  () const { return pimpl->sb.st_uid;   }
int LsStat::get_gid  () const { return pimpl->sb.st_gid;   }

std::string LsStat::get_user() const
{
	// FIXME
	return std::to_string(pimpl->sb.st_uid);
}

std::string LsStat::get_group() const
{
	// FIXME
	return std::to_string(pimpl->sb.st_gid);
}

long long LsStat::get_size() const
{
	return pimpl->sb.st_size;
}

struct stat &LsStat::get_stat()
{
	return pimpl->sb;
}

const struct stat &LsStat::get_stat() const
{
	return pimpl->sb;
}
