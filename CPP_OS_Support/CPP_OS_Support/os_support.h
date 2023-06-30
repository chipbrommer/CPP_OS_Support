///////////////////////////////////////////////////////////////////////////////
//!
//! @file		os_support.h
//! 
//! @brief		A cross platform class to communicate to the host OS and
//!				simplify access to common needed aspects. 
//! 
//! @author		Chip Brommer
//!
///////////////////////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Includes:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <string>						// Strings
#include <iostream>						// IO
#include <sstream>						// String stream
#include <map>							// Error map
//
#ifdef _WIN32
#include <Windows.h>
#include <psapi.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include <Pdh.h>
#pragma comment(lib, "Pdh.lib")
#elif __linux__
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#elif __APPLE__
#include <sys/mount.h>
#include <sys/sysinfo.h>
#include <sys/sysctl.h>
#include <mach/mach.h>
#endif
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CPP_OS_SUPPORT				// Define the os support class.
#define     CPP_OS_SUPPORT
// 
///////////////////////////////////////////////////////////////////////////////

namespace Essentials
{
	namespace Utilities
	{
		const static uint8_t OS_SUPPORT_VERSION_MAJOR = 0;
		const static uint8_t OS_SUPPORT_VERSION_MINOR = 1;
		const static uint8_t OS_SUPPORT_VERSION_PATCH = 0;
		const static uint8_t OS_SUPPORT_VERSION_BUILD = 0;

		/// @brief Printable string of the OS support version
		const static std::string OSSupportVersion = "OS Support v" +
			std::to_string(OS_SUPPORT_VERSION_MAJOR) + "." +
			std::to_string(OS_SUPPORT_VERSION_MINOR) + "." +
			std::to_string(OS_SUPPORT_VERSION_PATCH) + " - b" +
			std::to_string(OS_SUPPORT_VERSION_BUILD) + ".\n";

		/// @brief OS Support enum for error codes
		enum class SupportError : uint8_t
		{
			NONE,
		};

		/// @brief Error enum to readable string conversion map
		static std::map<SupportError, std::string> SupportErrorMap
		{
			{SupportError::NONE,
				std::string("Error Code " + std::to_string((uint8_t)SupportError::NONE) + ": No error.")},
		};

		class OS_Support
		{
		public:
			OS_Support();
			~OS_Support();
			double		GetCpuUsagePercent();
			double		GetTotalRamInGigabytes();
			uint64_t	GetTotalRamInBytes();
			uint64_t	GetFreeRamInBytes();
			uint64_t	GetUsedRamInBytes();
			double		GetRamUsagePercent();
			uint64_t	GetTotalDiskSpaceInBytes();
			double		GetTotalDiskSpaceInGigabytes();
			uint64_t	GetFreeDiskSpaceInBytes();
			double		GetFreeDiskSpaceInGigabytes();
			double		GetFreeDiskSpacePercent();
			double		GetUsedDiskSpacePercent();
			int			GetNumberOfEthernetDevices();
			int			MountStorageDevice(const std::string& device, const std::string& location);
			int			UnmountStorageDevice(const std::string& location);
			uint64_t	GetSystemUpTimeInSeconds();
			int			GetSystemUpTimeHMS(int& hours, int& mins, int& secs);
			std::string GetLastError();
		protected:
		private:
			SupportError	mLastError;
		};
	}
}
#endif