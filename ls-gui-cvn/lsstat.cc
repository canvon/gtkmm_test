#include "lsstat.hh"
#include <stdexcept>
#include <iomanip>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


class LsStat::impl
{
public:
	struct stat sb;  // stat buffer
};

LsStat::LsStat(const char *pathname)
{
	if (!pathname)
		throw std::invalid_argument("LsStat ctor: argument pathname is required");

	// Initialize smartpointer.
	pimpl = std::make_shared<LsStat::impl>();

	if (stat(pathname, &pimpl->sb)) {
		std::ostringstream os;
		os << "LsStat ctor: syscall stat() failed for " << std::quoted(pathname);
		if (errno)
			os << ": " << strerror(errno);
		throw std::runtime_error(os.str());
	}
}

bool LsStat::get_is_reg()  { return S_ISREG (pimpl->sb.st_mode); }
bool LsStat::get_is_dir()  { return S_ISDIR (pimpl->sb.st_mode); }
bool LsStat::get_is_chr()  { return S_ISCHR (pimpl->sb.st_mode); }
bool LsStat::get_is_blk()  { return S_ISBLK (pimpl->sb.st_mode); }
bool LsStat::get_is_fifo() { return S_ISFIFO(pimpl->sb.st_mode); }
bool LsStat::get_is_lnk()  { return S_ISLNK (pimpl->sb.st_mode); }
bool LsStat::get_is_sock() { return S_ISSOCK(pimpl->sb.st_mode); }

bool LsStat::get_filemode_set_uid()  { return (pimpl->sb.st_mode & S_ISUID) == S_ISUID; }
bool LsStat::get_filemode_set_gid()  { return (pimpl->sb.st_mode & S_ISGID) == S_ISGID; }
bool LsStat::get_filemode_sticky()   { return (pimpl->sb.st_mode & S_ISVTX) == S_ISVTX; }
bool LsStat::get_filemode_r_user()   { return (pimpl->sb.st_mode & S_IRUSR) == S_IRUSR; }
bool LsStat::get_filemode_w_user()   { return (pimpl->sb.st_mode & S_IWUSR) == S_IWUSR; }
bool LsStat::get_filemode_x_user()   { return (pimpl->sb.st_mode & S_IXUSR) == S_IXUSR; }
bool LsStat::get_filemode_r_group()  { return (pimpl->sb.st_mode & S_IRGRP) == S_IRGRP; }
bool LsStat::get_filemode_w_group()  { return (pimpl->sb.st_mode & S_IWGRP) == S_IWGRP; }
bool LsStat::get_filemode_x_group()  { return (pimpl->sb.st_mode & S_IXGRP) == S_IXGRP; }
bool LsStat::get_filemode_r_other()  { return (pimpl->sb.st_mode & S_IROTH) == S_IROTH; }
bool LsStat::get_filemode_w_other()  { return (pimpl->sb.st_mode & S_IWOTH) == S_IWOTH; }
bool LsStat::get_filemode_x_other()  { return (pimpl->sb.st_mode & S_IXOTH) == S_IXOTH; }

int LsStat::get_filemode_rwx_user()  { return pimpl->sb.st_mode & S_IRWXU; }
int LsStat::get_filemode_rwx_group() { return pimpl->sb.st_mode & S_IRWXG; }
int LsStat::get_filemode_rwx_other() { return pimpl->sb.st_mode & S_IRWXO; }

std::string LsStat::get_mode_str()
{
	// FIXME
	return "N.A.";
}

int LsStat::get_nlink() { return pimpl->sb.st_nlink; }

int LsStat::get_uid()   { return pimpl->sb.st_uid; }
int LsStat::get_gid()   { return pimpl->sb.st_gid; }

std::string LsStat::get_user()
{
	// FIXME
	return "N.A.";
}

std::string LsStat::get_group()
{
	// FIXME
	return "N.A.";
}

long long LsStat::get_size()
{
	return pimpl->sb.st_size;
}

struct stat &LsStat::get_stat()
{
	return pimpl->sb;
}
