#ifndef DIRENT_CVN_HH
#define DIRENT_CVN_HH

#include <memory>
#include <string>

//namespace cvn::fs
namespace cvn { namespace fs
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

		std::string get_ent_name();

		enum class EntType {
			Unknown,
			Regular,
			Directory,
			Symlink,
			BlockDevice,
			CharacterDevice,
			NamedPipe,
			Fifo = NamedPipe,
			Socket,
		};
		EntType get_ent_type();

	private:
		class impl;
		std::shared_ptr<impl> pimpl;
	};

}  // cvn::fs
}  // cvn, when not able to use nested namespace declarations from C++17 or GCC6

#endif  // DIRENT_CVN_HH
