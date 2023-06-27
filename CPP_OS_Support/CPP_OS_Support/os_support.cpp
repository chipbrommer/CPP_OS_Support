///////////////////////////////////////////////////////////////////////////////
//!
//! @file		os_support.cpp
//! 
//! @brief		Implementation of the os support class
//! 
//! @author		Chip Brommer
//!
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#include	"os_support.h"				// OS Support Class
//
///////////////////////////////////////////////////////////////////////////////

namespace Essentials
{
	namespace Utilities
	{
		OS_Support::OS_Support()
		{
			mLastError = SupportError::NONE;
		}

		OS_Support::~OS_Support()
		{

		}

		double OS_Support::GetTotalDiskSpaceInBytes()
		{
			double totalSpace = 0.0;

#ifdef WIN32
			// Windows implementation
			ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
			if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
			{
				totalSpace = static_cast<double>(totalBytes.QuadPart);
			}

#elif __linux__
			// Linux implementation
			struct statvfs diskInfo {};
			if (statvfs("/", &diskInfo) == 0)
			{
				totalSpace = static_cast<double>(diskInfo.f_frsize) * diskInfo.f_blocks;
			}

#elif __APPLE__
			// macOS implementation
			struct statfs diskInfo {};
			if (statfs("/", &diskInfo) == 0)
			{
				totalSpace = static_cast<double>(diskInfo.f_bsize) * diskInfo.f_blocks;
			}

#endif

			return totalSpace;
		}

		double OS_Support::GetFreeDiskSpaceInBytes()
		{
			double freeSpace = 0.0;

#ifdef _WIN32
			// Windows implementation
			ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
			if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
			{
				freeSpace = static_cast<double>(totalFreeBytes.QuadPart);
			}

#elif __linux__
			// Linux implementation
			struct statvfs diskInfo {};
			if (statvfs("/", &diskInfo) == 0)
			{
				freeSpace = static_cast<double>(diskInfo.f_frsize) * diskInfo.f_bavail;
			}

#elif __APPLE__
			// macOS implementation
			struct statfs diskInfo {};
			if (statfs("/", &diskInfo) == 0)
			{
				freeSpace = static_cast<double>(diskInfo.f_bsize) * diskInfo.f_bavail;
			}

#endif

			return freeSpace;
		}

		double OS_Support::GetFreeDiskSpacePercent()
		{
			double totalDiskSpace = GetTotalDiskSpaceInBytes();
			double freeDiskSpace = GetFreeDiskSpaceInBytes();
			double freeSpacePercentage = 0;

			if (totalDiskSpace != 0 && freeDiskSpace != 0)
			{
				(freeDiskSpace / totalDiskSpace) * 100.0;
			}

			return freeSpacePercentage;
		}

		int OS_Support::GetNumberOfEthernetDevices()
		{
			int result = -1;

#ifdef _WIN32
			// Windows implementation
			DWORD bufferLength = 0;

			// Get the size of the buffer needed
			if (GetAdaptersInfo(NULL, &bufferLength) == ERROR_BUFFER_OVERFLOW)
			{
				// Allocate memory for the adapter info
				PIP_ADAPTER_INFO adapters = reinterpret_cast<PIP_ADAPTER_INFO>(new char[bufferLength]);

				// Get the adapter info
				if (GetAdaptersInfo(adapters, &bufferLength) == NO_ERROR)
				{
					// Count the number of Ethernet adapters excluding loopback interfaces
					int count = 0;
					PIP_ADAPTER_INFO currentAdapter = adapters;
					while (currentAdapter)
					{
						if (currentAdapter->Type == MIB_IF_TYPE_ETHERNET && strcmp(currentAdapter->Description, "Loopback") != 0)
							count++;

						currentAdapter = currentAdapter->Next;
					}

					result = count;
				}

				delete[] reinterpret_cast<char*>(adapters);
			}

#elif __linux__
			// Linux implementation
			std::ifstream netDevFile("/proc/net/dev");
			if (netDevFile)
			{
				std::string line;
				int count = 0;

				// Count the number of lines that start with "eth" or "en" and exclude loopback interfaces
				while (std::getline(netDevFile, line))
				{
					if ((line.find("eth") == 0 || line.find("en") == 0) && line.find("lo:") != 0)
						count++;
				}

				netDevFile.close();
				result = count;
			}

#elif __APPLE__
			// macOS implementation
			int mib[6] = { CTL_NET, AF_ROUTE, 0, AF_LINK, NET_RT_IFLIST, 0 };
			size_t length;

			if (sysctl(mib, 6, NULL, &length, NULL, 0) == 0)
			{
				char* buffer = new char[length];
				if (sysctl(mib, 6, buffer, &length, NULL, 0) == 0)
				{
					struct if_msghdr* ifm = reinterpret_cast<struct if_msghdr*>(buffer);
					struct sockaddr_dl* sdl = reinterpret_cast<struct sockaddr_dl*>(ifm + 1);
					int count = 0;

					// Count the number of Ethernet devices excluding loopback interfaces
					while (sdl->sdl_type == IFT_ETHER)
					{
						if (sdl->sdl_index != 1) // Exclude loopback interface (index 1)
							count++;

						ifm = reinterpret_cast<struct if_msghdr*>(reinterpret_cast<char*>(sdl) + sdl->sdl_len);
						sdl = reinterpret_cast<struct sockaddr_dl*>(ifm + 1);
					}

					result = count;
				}

				delete[] buffer;
			}

#endif

			return result;
	}

		int OS_Support::MountStorageDevice(const std::string& device, const std::string& location)
		{
			int result = -1;

#ifdef _WIN32
			// Windows implementation
			std::cout << "Mounting storage devices is not applicable on Windows." << std::endl;
			result = 0;

#elif __linux__
			// Linux implementation
			std::string checkCommand = "grep -qs " + location + " /proc/mounts";
			int checkResult = std::system(checkCommand.c_str());

			if (checkResult == 0)
			{
				std::cout << "Drive is already mounted." << std::endl;
				result = 0;
			}
			else
			{
				std::string mountCommand = "mount " + device + " " + location;
				int mountResult = std::system(mountCommand.c_str());
				if (mountResult == 0)
				{
					std::cout << "Drive mounted successfully." << std::endl;
					result = 1;
				}
				else
				{
					std::cout << "Failed to mount drive." << std::endl;
					result = -1;
				}
			}

#elif __APPLE__
			// macOS implementation
			std::string checkCommand = "diskutil list | grep -qs " + location;
			int checkResult = std::system(checkCommand.c_str());

			if (checkResult == 0)
			{
				std::cout << "Drive is already mounted." << std::endl;
				result = 0;
			}
			else
			{
				std::string mountCommand = "diskutil mount " + device + " " + location;
				int mountResult = std::system(mountCommand.c_str());
				if (mountResult == 0)
				{
					std::cout << "Drive mounted successfully." << std::endl;
					result = 1;
				}
				else
				{
					std::cout << "Failed to mount drive." << std::endl;
					result = -1;
				}
			}

#endif

			return result;
		}

		int OS_Support::UnmountStorageDevice(const std::string& location)
		{
			int result = -1;

#ifdef _WIN32
			// Windows implementation
			std::cout << "Unmounting storage devices is not applicable on Windows." << std::endl;
			result = 0;

#elif __linux__
			// Linux implementation
			std::string checkCommand = "grep -qs " + location + " /proc/mounts";
			int checkResult = std::system(checkCommand.c_str());

			if (checkResult == 0)
			{
				std::string unmountCommand = "umount " + location;
				int unmountResult = std::system(unmountCommand.c_str());
				if (unmountResult == 0)
				{
					std::cout << "Drive unmounted successfully." << std::endl;
					result = 1;
				}
				else
				{
					std::cout << "Failed to unmount folder." << std::endl;
					result = -1;
				}
			}
			else
			{
				std::cout << "Location is not mounted." << std::endl;
				result = 0;
			}

#elif __APPLE__
			// macOS implementation
			std::string checkCommand = "diskutil list | grep -qs " + location;
			int checkResult = std::system(checkCommand.c_str());

			if (checkResult == 0)
			{
				std::string unmountCommand = "diskutil unmount " + location;
				int unmountResult = std::system(unmountCommand.c_str());
				if (unmountResult == 0)
				{
					std::cout << "Drive unmounted successfully." << std::endl;
					result = 1;
				}
				else
				{
					std::cout << "Failed to unmount folder." << std::endl;
					result = -1;
				}
			}
			else
			{
				std::cout << "Location is not mounted." << std::endl;
				result = 0;
			}

#endif

			return result;
		}

		int OS_Support::GetSystemUpTimeInSeconds()
		{
			uint64_t uptime = 0;

#ifdef WIN32
			// Windows implementation
			ULONGLONG msCount = GetTickCount64();
			uptime = msCount / 1000;
#elif __linux__ || __APPLE__
			// Linux and macOS implementation
			struct sysinfo info {};
			if (sysinfo(&info) == 0)
			{
				uptime = info.uptime;
			}
#endif

			return uptime;
		}

		int OS_Support::GetSystemUpTimeHMS(int& hours, int& mins, int& secs)
		{
			int uptime = GetSystemUpTimeInSeconds();

			if (uptime > 0)
			{
				mins = uptime / 60;
				hours = mins / 60;
				mins = mins % 60;
				secs = uptime % 60;

				return 0;
			}

			// default return is fail. 
			return -1;
		}

		std::string OS_Support::GetLastError()
		{
			return SupportErrorMap[mLastError];
		}
	}
}
