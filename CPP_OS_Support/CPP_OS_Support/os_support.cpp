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

		double OS_Support::GetTotalRamInGigabytes()
		{
			double totalRAM = 0.0;

#ifdef _WIN32
			MEMORYSTATUSEX memoryStatus{};
			memoryStatus.dwLength = sizeof(memoryStatus);
			if (GlobalMemoryStatusEx(&memoryStatus))
			{
				totalRAM = static_cast<double>(memoryStatus.ullTotalPhys);
			}

#elif __linux__
			std::ifstream meminfoFile("/proc/meminfo");

			if (meminfoFile.is_open())
			{
				std::string line;
				while (std::getline(meminfoFile, line))
				{
					if (line.find("MemTotal:") != std::string::npos)
					{
						double totalMemory;
						std::sscanf(line.c_str(), "MemTotal: %lf", &totalMemory);
						totalRAM = totalMemory * 1024.0;  // Convert from kilobytes to bytes
						break;
					}
				}
				meminfoFile.close();
			}

#elif __APPLE__
			int mib[2];
			size_t length;
			mib[0] = CTL_HW;
			mib[1] = HW_MEMSIZE;
			length = sizeof(totalRAM);
			sysctl(mib, 2, &totalRAM, &length, nullptr, 0);

#endif

			return totalRAM / (1024.0 * 1024.0 * 1024.0);
		}

		uint64_t OS_Support::GetTotalRamInBytes()
		{
			uint64_t totalRAM = 0;

#ifdef _WIN32
			// Windows implementation
			MEMORYSTATUSEX memoryStatus{};
			memoryStatus.dwLength = sizeof(memoryStatus);
			if (GlobalMemoryStatusEx(&memoryStatus))
			{
				totalRAM = static_cast<uint64_t>(memoryStatus.ullTotalPhys);
			}
#elif __linux__
			// Linux implementation
			struct sysinfo info;
			if (sysinfo(&info) != -1)
			{
				totalRAM = static_cast<uint64_t>(info.totalram * info.mem_unit);
			}
#elif __APPLE__
			// macOS implementation
			int mib[2];
			mib[0] = CTL_HW;
			mib[1] = HW_MEMSIZE;
			uint64_t physicalMemory;
			uint64_t length = sizeof(physicalMemory);
			sysctl(mib, 2, &physicalMemory, &length, nullptr, 0);
			totalRAM = physicalMemory;
#endif

			return totalRAM;
		}

		uint64_t OS_Support::GetFreeRamInBytes()
		{
			uint64_t freeRAM = 0;

#ifdef _WIN32
			// Windows implementation
			MEMORYSTATUSEX memoryStatus{};
			memoryStatus.dwLength = sizeof(memoryStatus);
			if (GlobalMemoryStatusEx(&memoryStatus))
			{
				freeRAM = static_cast<uint64_t>(memoryStatus.ullAvailPhys);
			}
#elif __linux__
			// Linux implementation
			struct sysinfo info;
			if (sysinfo(&info) != -1)
			{
				freeRAM = static_cast<uint64_t>(info.freeram * info.mem_unit);
			}
#elif __APPLE__
			// macOS implementation
			int64_t physicalMemory;
			size_t length = sizeof(physicalMemory);
			sysctlbyname("hw.memsize", &physicalMemory, &length, nullptr, 0);
			freeRAM = static_cast<size_t>(physicalMemory);
#endif

			return freeRAM;
		}

		uint64_t OS_Support::GetUsedRamInBytes()
		{
			uint64_t ramUsage = 0;

#ifdef _WIN32
			// Windows implementation
			PROCESS_MEMORY_COUNTERS_EX pmc{};
			if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
			{
				ramUsage = pmc.WorkingSetSize;
			}
#elif __linux__
			std::ifstream meminfoFile("/proc/meminfo");
			std::string line;

			while (std::getline(meminfoFile, line))
			{
				// Search for the "MemAvailable" or "Active" line
				if (line.compare(0, 12, "MemAvailable") == 0 || line.compare(0, 6, "Active") == 0)
				{
					std::istringstream iss(line);
					std::string fieldName;
					size_t fieldValue;

					// Extract the field name and value
					iss >> fieldName >> fieldValue;

					if (fieldName == "MemAvailable:" || fieldName == "Active:")
					{
						// Convert from kilobytes to bytes
						ramUsage = fieldValue * 1024;
						break;
					}
		}
			}
#elif __APPLE__
			// macOS implementation
			struct mach_task_basic_info info;
			mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;
			if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) == KERN_SUCCESS)
			{
				ramUsage = info.resident_size;
			}
#endif

			return ramUsage;
		}

		double OS_Support::GetRamUsagePercent()
		{
			uint64_t totalRAM = GetTotalRamInBytes();
			uint64_t freeRAM = GetFreeRamInBytes();
			uint64_t usedRAM = totalRAM - freeRAM;
			double percent = static_cast<double>(usedRAM) / totalRAM;

			return percent * 100.0;
		}

		uint64_t OS_Support::GetTotalDiskSpaceInBytes()
		{
			uint64_t totalSpace = 0;

#ifdef WIN32
			// Windows implementation
			ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
			if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
			{
				totalSpace = static_cast<uint64_t>(totalBytes.QuadPart);
			}

#elif __linux__
			// Linux implementation
			struct statvfs diskInfo {};
			if (statvfs("/", &diskInfo) == 0)
			{
				totalSpace = static_cast<uint64_t>(diskInfo.f_frsize) * diskInfo.f_blocks;
			}

#elif __APPLE__
			// macOS implementation
			struct statfs diskInfo {};
			if (statfs("/", &diskInfo) == 0)
			{
				totalSpace = static_cast<uint64_t>(diskInfo.f_bsize) * diskInfo.f_blocks;
			}

#endif

			return totalSpace;
		}

		double OS_Support::GetTotalDiskSpaceInGigabytes()
		{
			double bytes = (double)GetTotalDiskSpaceInBytes();

			return bytes / (1024 * 1024 * 1024);
		}

		uint64_t OS_Support::GetFreeDiskSpaceInBytes()
		{
			uint64_t freeSpace = 0;

#ifdef _WIN32
			// Windows implementation
			ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
			if (GetDiskFreeSpaceEx(NULL, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
			{
				freeSpace = static_cast<uint64_t>(totalFreeBytes.QuadPart);
			}

#elif __linux__
			// Linux implementation
			struct statvfs diskInfo {};
			if (statvfs("/", &diskInfo) == 0)
			{
				freeSpace = static_cast<uint64_t>(diskInfo.f_frsize) * diskInfo.f_bavail;
			}

#elif __APPLE__
			// macOS implementation
			struct statfs diskInfo {};
			if (statfs("/", &diskInfo) == 0)
			{
				freeSpace = static_cast<uint64_t>(diskInfo.f_bsize) * diskInfo.f_bavail;
			}

#endif

			return freeSpace;
		}

		double OS_Support::GetFreeDiskSpaceInGigabytes()
		{
			double bytes = (double)GetFreeDiskSpaceInBytes();

			return bytes / (1024 * 1024 * 1024);
		}

		double OS_Support::GetFreeDiskSpacePercent()
		{
			double totalDiskSpace = (double)GetTotalDiskSpaceInBytes();
			double freeDiskSpace = (double)GetFreeDiskSpaceInBytes();
			double freeSpacePercentage = 0;

			if (totalDiskSpace != 0 && freeDiskSpace != 0)
			{
				freeSpacePercentage = (freeDiskSpace / totalDiskSpace) * 100.0;
			}

			return freeSpacePercentage;
		}

		double OS_Support::GetUsedDiskSpacePercent()
		{
			double totalDiskSpace = (double)GetTotalDiskSpaceInBytes();
			double freeDiskSpace = (double)GetFreeDiskSpaceInBytes();
			double usedDiskSpace = totalDiskSpace - freeDiskSpace;
			double usedSpacePercentage = 0;

			if (totalDiskSpace != 0 && freeDiskSpace != 0)
			{
				usedSpacePercentage = (usedDiskSpace / totalDiskSpace) * 100.0;
			}

			return usedSpacePercentage;
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
					// Count the number of Ethernet adapters excluding loopback interfaces, virtual, and Bluetooth adapters
					int count = 0;
					PIP_ADAPTER_INFO currentAdapter = adapters;
					while (currentAdapter)
					{
						if (currentAdapter->Type == MIB_IF_TYPE_ETHERNET)
						{
							std::string adapterDescription(currentAdapter->Description);

							if (adapterDescription.find("Virtual") == std::string::npos		&& 
								adapterDescription.find("Bluetooth") == std::string::npos	&&
								adapterDescription.find("Loopback") == std::string::npos)
							{
								count++;
							}
						}

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
			std::string checkCommand = "mount | grep -qs '" + device + "\\|" + location + "'";
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
			std::string checkCommand = = "diskutil info " + device + " | grep -q 'Mount Point: " + location + "'";
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

		uint64_t OS_Support::GetSystemUpTimeInSeconds()
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
			uint64_t uptime = GetSystemUpTimeInSeconds();

			if (uptime > 0)
			{
				mins = (int)uptime / 60;
				hours = mins / 60;
				mins = mins % 60;
				secs = (int)uptime % 60;

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
