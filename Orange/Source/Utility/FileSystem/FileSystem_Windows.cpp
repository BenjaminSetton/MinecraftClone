
#include <codecvt>
#include <filesystem> // Include the Windows filesystem API
#include <locale>

#include "FileSystem_Windows.h"
#include "FileSystem.h"
#include "../Utility.h"

using namespace std::filesystem;

namespace Orange
{
	FileSystem_Windows::FileSystem_Windows() { }

	FileSystem_Windows::FileSystem_Windows(const FileSystem_Windows& other) { UNUSED(other); }

	FileSystem_Windows::~FileSystem_Windows() { }

	void FileSystem_Windows::Initialize()
	{
		// Calculate and stash the project directory
		m_rootDirectory = CalculateRootDirectory();

		// Calculate and stash the directories and filenames
		PopulateMaps();
	}

	const bool FileSystem_Windows::DoesFileExist(const std::string fileName, bool considerExtension) const
	{
		bool found = false;
		auto name = path(fileName);
		if (considerExtension)
		{
			OG_ASSERT_MSG(name.has_extension(), "Attempting to check if file exists considering extension without passing in an extension");
			if (m_files.contains(fileName))
			{
				found = true;
			}
		}
		else // !considerExtension
		{
			// Unfortunately we have to loop over the map to check for an extension-less match.
			// This will have to do for now
			// TODO - Find a better way to do this. Perhaps we could modify the map so that we check
			//        matches only with the filename, and we can then retrieve the extension afterwards
			OG_ASSERT_MSG(!name.has_extension(), "Attempting to check if file exists excluding extension by passing in an extension");
			for (auto iter = m_files.begin(); iter != m_files.end(); iter++)
			{
				std::string nameWithoutExtension = path(iter->first).stem().string();
				if (fileName == nameWithoutExtension)
				{
					found = true;
				}
			}
		}
		return found;
	}

	const bool FileSystem_Windows::DoesDirectoryExist(const std::string directory) const
	{
		return m_directories.contains(directory);
	}

	std::string FileSystem_Windows::CalculateRootDirectory() const
	{
		// Move into the Source directory - or the project directory
		return current_path().parent_path().append("Source").string();
	}

	const std::string FileSystem_Windows::GetFileNameRelativeToGeneratedDirectory(const std::string& fileName)
	{
		return (std::string("..\\Source\\") + m_files[fileName]);
	}

	const std::wstring FileSystem_Windows::GetFileNameRelativeToGeneratedDirectory(const std::wstring& fileName)
	{
		return StringToWideString((std::string("..\\Source\\") + m_files[WideStringToString(fileName)])).c_str();
	}

	void FileSystem_Windows::PopulateMaps()
	{
		std::string lastDirectory = "";
		for (recursive_directory_iterator curr(path(GetRootDirectory().c_str())), end; curr != end; curr++)
		{
			std::string root = GetRootDirectory();
			std::string currPath = curr->path().string();
			std::filesystem::path relativePathToProjectDir = relative(currPath, GetRootDirectory());

			std::string fileName = relativePathToProjectDir.filename().string();
			bool isDirectory = !relativePathToProjectDir.has_extension();

			if (isDirectory)
			{
				m_directories[fileName] = relativePathToProjectDir.string();
			}
			else
			{
				m_files[fileName] = relativePathToProjectDir.string();
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////
	// 
	// Private utility functions
	//
	//////////////////////////////////////////////////////////////////////////////

	std::wstring FileSystem_Windows::StringToWideString(const std::string& str)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string FileSystem_Windows::WideStringToString(const std::wstring& wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}

}


