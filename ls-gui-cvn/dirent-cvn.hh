#ifndef DIRENT_CVN_HH
#define DIRENT_CVN_HH

#include <memory>
#include <string>

namespace cvn::fs
{

class Dirent
{
public:
	Dirent(const char *pathname);
	Dirent(const std::string &pathname_str);
	~Dirent();

	void close();

	int fd();

	// Returns true if a directory entry has been read,
	//         false if the end of the directory stream has been reached.
	// Other cases will likely throw an exception.
	bool read();

	std::string get_name();

private:
	class impl;
	std::shared_ptr<impl> pimpl;
};

}

#endif  // DIRENT_CVN_HH
