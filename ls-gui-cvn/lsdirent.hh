#ifndef LS_DIRENT_HH
#define LS_DIRENT_HH

#include <memory>
#include <string>

class LsDirent
{
public:
	LsDirent(const char *pathname);
	~LsDirent();

	void close();

	// Returns true if a directory entry has been read,
	//         false if the end of the directory stream has been reached.
	// Other cases will likely throw an exception.
	bool read();

	std::string get_name();

private:
	class impl;
	std::shared_ptr<impl> pimpl;
};

#endif  // LS_DIRENT_HH
