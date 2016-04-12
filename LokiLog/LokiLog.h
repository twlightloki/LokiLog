#ifndef LOKILOG_H
#define LOKILOG_H

#include <Windows.h>
#include <stdarg.h>
#include <thread>
#include <stdio.h>

#define MAXIMUM_BUFFER 1024
#define BUFFER_LEN 128

struct BufferNode
{
	CHAR cBuffer[MAXIMUM_BUFFER];
	BOOL bAvail;
};

struct LokiLogThreadParam
{
	HANDLE fTarget;
	BufferNode nBufferList[BUFFER_LEN];
	INT nOpen, nClose;
	BOOL bThreadStatus;
};

class LokiLog
{
public:
	LokiLog();
	~LokiLog();

	BOOL SetName(LPCWSTR wstrFileName, BOOL bTimeStamp);

	BOOL StartLog();

	BOOL StopLog();

	BOOL ClearLog();

	BOOL WriteLog(LPCSTR strFormat, ...);

private:
	WCHAR wstrFileName[256];

	BOOL bNameDefine, bLogStart, bTimeStamp;

	LokiLogThreadParam pThread;

	std::thread tMain;


	BOOL WriteToBuffer(LPCSTR strFormat, va_list arg);

	INT BufferStatus();

	static void ThreadExecution(LokiLogThreadParam *pThread);





};

#endif