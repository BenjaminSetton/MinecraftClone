
#include "FileSystem.h"
#include "FileSystem_Windows.h"
#include "../Utility.h"

namespace Orange
{
	FileSystem_Base* FileSystem::m_instance = nullptr;
	bool FileSystem::m_isInitialized = false;


	void FileSystem::Initialize()
	{
		// Sanity check for multiple init
		if (m_isInitialized)
		{
			OG_ASSERT_MSG(false, "FileSystem::Init() is being called when it's already initialized!");
		}

		// TODO - Add other platforms
	#if defined(OG_WINDOWS)

		m_instance = new FileSystem_Windows();

	#endif
	
		// Allow the virtual derived class to Initialize as well
		m_instance->Initialize();

		m_isInitialized = true;
	}

	const bool FileSystem::DoesFileExist(const std::string fileName, bool considerExtension)
	{
		if (m_instance)
		{
			return m_instance->DoesFileExist(fileName, considerExtension);
		}

		OG_ASSERT_MSG(false, "FileSystem has not been initialized!");
		return false;
	}

	const bool FileSystem::DoesDirectoryExist(const std::string directory)
	{
		if (m_instance)
		{
			return m_instance->DoesDirectoryExist(directory);
		}

		OG_ASSERT_MSG(false, "FileSystem has not been initialized!");
		return false;
	}

	const std::string FileSystem::GetRootDirectory()
	{
		if (m_instance)
		{
			return m_instance->GetRootDirectory();
		}

		OG_ASSERT_MSG(false, "FileSystem has not been initialized!");
		return "";
	}

	const std::string FileSystem::GetFileNameRelativeToGeneratedDirectory(const std::string& fileName)
	{
		if (m_instance)
		{
			return m_instance->GetFileNameRelativeToGeneratedDirectory(fileName);
		}

		OG_ASSERT_MSG(false, "FileSystem has not been initialized!");
		return "";
	}

	const std::wstring FileSystem::GetFileNameRelativeToGeneratedDirectory(const std::wstring& fileName)
	{
		if (m_instance)
		{
			return m_instance->GetFileNameRelativeToGeneratedDirectory(fileName);
		}

		OG_ASSERT_MSG(false, "FileSystem has not been initialized!");
		return L"";
	}
}


