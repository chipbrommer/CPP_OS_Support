#include <iostream>
#include "CPP_OS_Support/os_support.h"

int main()
{
	Essentials::Utilities::OS_Support os;

	std::cout << "Time Up:      " << os.GetSystemUpTimeInSeconds()		<< " secs\n";

	int secs, mins, hrs;
	int rtn = os.GetSystemUpTimeHMS(hrs, mins, secs);

	if (rtn > -1)
	{
		std::cout << hrs << ":" << mins << ":" << secs << "\n";
	}

	std::cout << "\n\n";

	std::cout << "Total Ram:    " << os.GetTotalRamInGigabytes()		<< " Gb\n";
	std::cout << "Total Space:  " << os.GetTotalDiskSpaceInGigabytes()	<< " Gb\n";
	std::cout << "Free Space:   " << os.GetFreeDiskSpaceInGigabytes()	<< " Gb\n";
	std::cout << "Percent Free: " << os.GetFreeDiskSpacePercent()		<< " %\n";

	std::cout << "\n\n";

	std::cout << "Number Eth Devices: " << os.GetNumberOfEthernetDevices() << "\n";

	return 0;
}
