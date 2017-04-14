#ifndef LS_STAT_HH
#define LS_STAT_HH

#include <memory>

class LsStat
{
public:
	LsStat(const char *pathname);

	struct stat &get_stat();

private:
	class impl;
	std::shared_ptr<impl> pimpl;
};

#endif  // LS_STAT_HH
