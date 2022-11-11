#include "../Misc/pch.h"

#include "../Utility/HeapOverrides.h"
#include "Log.h"
#include "../Utility/Utility.h"
#include <cstdarg>


Log::Log() : mLastWritePos(0), mOutputFile(nullptr), mNewMessage(true), mTimestamps(true), 
			 mNewLineAfterMessage(true), m_colors(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
{
	// Console HANDLE
	m_console = GetStdHandle(STD_OUTPUT_HANDLE);
}

Log::~Log()
{
	if (mOutputFile) delete mOutputFile;
}

void Log::SetOutputFile(const char* path)
{
	if (mOutputFile)
	{
		delete[] mOutputFile;
		mOutputFile = nullptr;
	}

	uint32_t filePathSize = static_cast<uint32_t>(strlen(path));

	mOutputFile = OG_NEW char[filePathSize + 1];
	memcpy(mOutputFile, path, filePathSize);
	mOutputFile[filePathSize] = 0;

	//strncpy_s(mOutputFile, strlen(filePath) + 1, filePath, strlen(filePath));
	//strncpy_s(&mOutputFile[0], strlen(filePathSize) + 1, path, strlen(path));
}

const char* Log::GetOutputFile()
{
	return mOutputFile;
}

void Log::PrintNLToFile(const char* _msg, ...)
{
	va_list varptr;
	va_start(varptr, _msg);

	int numArgs = _vscprintf(_msg, varptr);
	std::vector<char> buf(numArgs + 1);

	vsprintf(&buf[0], _msg, varptr);

	va_end(varptr);

	// Open a stream for decimal output
	std::ofstream stream(mOutputFile, std::ios::binary | std::ios::out | std::ios::app);

	OG_ASSERT(stream.is_open());

	stream.seekp(mLastWritePos);

	std::copy(buf.begin(), buf.end() - 1, std::ostream_iterator<char>(stream));

	char newLine = 0xA;
	stream.write(&newLine, sizeof(newLine));

	mLastWritePos = (long)stream.tellp();

	stream.close();
}

void Log::PrintTimestamp() 
{
	if (!mNewMessage || !mTimestamps) return;

	auto timeNow = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(timeNow);
	tm timeStruct;
	localtime_s(&timeStruct, &time);

	std::cout << "[";
	if (timeStruct.tm_hour < 10) std::cout << "0";
	std::cout << timeStruct.tm_hour << ":";
	if (timeStruct.tm_min < 10) std::cout << "0";
	std::cout << timeStruct.tm_min << ":";
	if (timeStruct.tm_sec < 10) std::cout << "0";
	std::cout << timeStruct.tm_sec;
	std::cout << "] ";
}

void Log::PrintToFile(const char* _msg, ...)
{
	va_list varptr;
	va_start(varptr, _msg);

	int numArgs = _vscprintf(_msg, varptr);
	std::vector<char> buf(numArgs + 1);

	vsprintf(&buf[0], _msg, varptr);

	va_end(varptr);

	// Open a stream for decimal output
	std::ofstream stream(mOutputFile, std::ios::binary | std::ios::out | std::ios::app);

	OG_ASSERT(stream.is_open());

	stream.seekp(mLastWritePos);

	std::copy(buf.begin(), buf.end() - 1, std::ostream_iterator<char>(stream));

	mLastWritePos = static_cast<long>(stream.tellp());

	stream.close();
}

void Log::End()
{
	mNewMessage = true;

	std::cout << "\n";
}

void Log::PrintIf(const char* _msg, bool _condition)
{
	if (_condition) Print(_msg);
}

void Log::PrintNLIf(const char* _msg, bool _condition)
{
	if (_condition) PrintNL(_msg);
}

void Log::SetDebugTimestamps(bool _state)
{
	mTimestamps = _state;
}

void Log::SetNewLineAfterMessage(bool _state)
{
	mNewLineAfterMessage = _state;
}

void Log::SetConsoleColors(WORD colors) 
{
	m_colors = colors;
	SetConsoleTextAttribute(m_console, m_colors);
}

const WORD& Log::GetConsoleColors() { return m_colors; }
