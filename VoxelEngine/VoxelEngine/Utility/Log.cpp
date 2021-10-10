#include "../Misc/pch.h"
#include "Log.h"


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

void Log::SetOutputFile(const char* fileName)
{
	if (mOutputFile)
	{
		delete[] mOutputFile;
		mOutputFile = nullptr;
	}

	const char* filePath = "../../../Debug Log/";
	uint32_t filePathSize = strlen(filePath);
	uint32_t fileNameSize = strlen(fileName);
	uint32_t outputFileSize = filePathSize + fileNameSize;

	mOutputFile = new char[outputFileSize + 1];

	strncpy_s(mOutputFile, strlen(filePath) + 1, filePath, strlen(filePath));
	strncpy_s(&mOutputFile[0] + filePathSize, strlen(fileName) + 1, fileName, strlen(fileName));
}

const char* Log::GetOutputFile()
{
	return mOutputFile;
}

void Log::PrintNLToFile(const char* _msg)
{
	// Open a stream for decimal output
	std::ofstream stream(mOutputFile, std::ios::binary | std::ios::out | std::ios::app);

	assert(stream.is_open());

	stream.seekp(mLastWritePos);

	stream.write(_msg, strlen(_msg));

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

void Log::PrintToFile(const char* _msg)
{
	// Open a stream for decimal output
	std::ofstream stream(mOutputFile, std::ios::binary | std::ios::out | std::ios::app);

	assert(stream.is_open());

	stream.seekp(mLastWritePos);

	stream.write(_msg, strlen(_msg));

	mLastWritePos = (long)stream.tellp();
	mLastWritePos++;

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
