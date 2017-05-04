#ifndef USERS_CVN_HH
#define USERS_CVN_HH

#include <memory>
#include <string>

namespace cvn
{

	class Users
	{
	public:
		Users();
		~Users();

		void close();
		void rewind();

		// Returns true if a password database entry has been read,
		//         false if the end of the password database has been reached.
		// Other cases will likely throw an exception.
		bool read();

		std::string get_ent_username();
		std::string get_ent_password();
		int get_ent_uid(), get_ent_gid();
		std::string get_ent_gecos();
		std::string get_ent_homedir();
		std::string get_ent_shell();

	private:
		class impl;
		std::unique_ptr<impl> pimpl;
	};

}  // cvn

#endif  // USERS_CVN_HH
