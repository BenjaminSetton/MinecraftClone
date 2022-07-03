#ifndef FILESYSTEM_BASE_H
#define FILESYSTEM_BASE_H

#include <string>
#include <unordered_map>

#include "../Utility.h"

namespace Orange
{
	// This is a virtual base class that platform-specific derived classes
	// will optionally implement to implement any filesystem functionality
	class FileSystem_Base
	{
	public:

		friend class FileSystem;

	protected:

		virtual void Initialize() { };

		virtual const bool DoesFileExist(const std::string fileName, bool considerExtension) const { UNUSED(considerExtension); return false; };
		virtual const bool DoesDirectoryExist(const std::string directory) const { return false; };
		virtual std::string CalculateRootDirectory() const { return std::string(""); };

		// Helper functions for initializing the directory and file maps
		virtual void PopulateMaps() { };

		// NON-VIRTUAL - THIS IS INTENTIONAL, I DON'T WANT TO ALLOW THE ROOT DIRECTORY TO BE CHANGED
		const std::string GetRootDirectory() { return m_rootDirectory; };

		virtual const std::string GetFileNameRelativeToGeneratedDirectory(const std::string& fileName) { UNUSED(fileName); return ""; };
		virtual const std::wstring GetFileNameRelativeToGeneratedDirectory(const std::wstring& fileName) { UNUSED(fileName); return L""; };

		// These unordered_maps map a directory or filename to their corresponding paths.
		// This allows us to quickly know whether a directory or file exists and retrieve their
		// path. The path is RELATIVE to the project directory, which is the root for all paths
		std::unordered_map<std::string, std::string> m_directories;
		std::unordered_map<std::string, std::string> m_files;

		std::string m_rootDirectory;

	};
}


#endif