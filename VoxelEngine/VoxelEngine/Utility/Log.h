#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <DirectXMath.h>
#include <windows.h>

class Log
{
public:

	Log();
	~Log();

	// I don't want to copy Log instances or assign to other instances
	Log(const Log& other) = delete;
	void operator=(const Log& other) = delete;

	void SetOutputFile(const char* _fileName);
	const char* GetOutputFile();

	template <typename type>
	void Print(const type& _value)
	{
		std::cout << _value;
	}

	template <typename type>
	void PrintNL(const type& _value)
	{
		Print(_value);
		std::cout << std::endl;
	}

	template<typename Type>
	Log& operator<< (const Type _msg)
	{
		// Prints out a debug time
		if (mNewMessage && mTimestamps)
		{
			auto timeNow = std::chrono::system_clock::now();
			std::time_t time = std::chrono::system_clock::to_time_t(timeNow);
			tm timeStruct;
			localtime_s(&timeStruct, &time);

			// Print out the debug timestamps
			std::cout << "[";
			if (timeStruct.tm_hour < 10) std::cout << "0";
			std::cout << timeStruct.tm_hour << ":";
			if (timeStruct.tm_min < 10) std::cout << "0";
			std::cout << timeStruct.tm_min << ":";
			if (timeStruct.tm_sec < 10) std::cout << "0";
			std::cout << timeStruct.tm_sec;
			std::cout << "] ";
		}

		std::cout << _msg;
		mNewMessage = false;

		return *this;
	}


	Log& operator<< (const DirectX::XMFLOAT3 vec);


	Log& operator<< (const DirectX::XMFLOAT4 vec);

	void End();

	void PrintToFile(const char* _msg);

	void PrintNLToFile(const char* _msg);

	void PrintIf(const char* _msg, bool _condition);

	void PrintNLIf(const char* _msg, bool _condition);
 
	void SetDebugTimestamps(bool _state);

	void SetNewLineAfterMessage(bool _state);

	// NOTE: The parameter is made up of flags that determine background and foreground color.
	//		 For example, FOREGROUND_RED | BACKGROUND_GREEN will print red text on a green bg.
	//		 If no color for foreground or background is specified, it will default to black,
	//		 and if all three RGB colors are specified it will result white
	void SetConsoleColors(WORD colors);
	const WORD& GetConsoleColors();

private:

	// Contains the name of the output file
	// User only has to enter the name of the file
	// Since the path is already hard-coded to end up in the Debug Log folder
	char* mOutputFile;

	// Contains the last writing position of the file
	long mLastWritePos;

	// Tracks the start of a message
	bool mNewMessage;

	// Tracks state of enabling/disabling timestamps
	bool mTimestamps;

	// Tracks state of enabling/disabling automatic new line character after each message
	bool mNewLineAfterMessage;

	// The console handle, this will be used to color the console text
	HANDLE m_console;

	// The colors that the console will use to print out text or the background
	WORD m_colors;

};

#endif

