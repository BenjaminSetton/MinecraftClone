#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>

// Forward declaration
class FileSystem_Base;

class FileSystem
{
public:

	static void Initialize();

	static const bool DoesFileExist(const std::string fileName, bool considerExtension = false);
	static const bool DoesDirectoryExist(const std::string directory);

	// Returns the root directory that all paths are relative to. 
	// This is the PROJECT directory, and this is the ONLY ABSOLUTE path
	static const std::string GetRootDirectory();

	// Returns the relative path of the filename to the root directory
	static const std::string GetFileNameRelativeToGeneratedDirectory(const std::string& fileName);

private:

	static FileSystem_Base* m_instance;

	static bool m_isInitialized;

};


#endif