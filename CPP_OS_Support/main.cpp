
#include <iostream>
#include "CPP_OS_Support/os_support.h"

int main()
{
	Essentials::Utilities::OS_Support os;
	std::cout << "Hello CMake." << std::endl;
	std::cout << os.GetSystemUpTimeInSeconds() << std::endl;
	int secs, mins, hrs;
	int rtn = os.GetSystemUpTimeHMS(hrs,mins,secs);

	if (rtn > -1)
	{
		std::cout << "\n\tHours: " << hrs
			<< "\n\tMins: " << mins
			<< "\n\tSecs: " << secs
			<< std::endl;
	}

	return 0;
}
