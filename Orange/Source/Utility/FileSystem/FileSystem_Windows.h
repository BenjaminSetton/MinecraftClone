#ifndef FILESYSTEM_WINDOWS_H
#define FILESYSTEM_WINDOWS_H

#include "FileSystem_Base.h"

namespace Orange
{
	class FileSystem_Windows : public FileSystem_Base
	{
	public:

		FileSystem_Windows();
		FileSystem_Windows(const FileSystem_Windows& other);
		~FileSystem_Windows();

		void Initialize() override;

		const bool DoesFileExist(const std::string fileName, bool considerExtension) const override;
		const bool DoesDirectoryExist(const std::string directory) const override;
		std::string CalculateRootDirectory() const override;

		const std::string GetFileNameRelativeToGeneratedDirectory(const std::string& fileName) override;
		const std::wstring GetFileNameRelativeToGeneratedDirectory(const std::wstring& fileName) override;

		// Internal - utility function for Initialize()
		void PopulateMaps() override;

	private:

		std::wstring StringToWideString(const std::string& str);
		std::string WideStringToString(const std::wstring& wstr);

	};
}


#endif