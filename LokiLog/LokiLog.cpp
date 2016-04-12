#include "LokiLog.h"


LokiLog::LokiLog()
{
	bNameDefine = FALSE;
	bLogStart = FALSE;
	bTimeStamp = FALSE;
	
}

LokiLog::~LokiLog()
{
	StopLog();
}

BOOL LokiLog::SetName(LPCWSTR wstrFileName, BOOL bTimeStamp)
{
	if (!bLogStart)
	{
		bNameDefine = TRUE;
		memset(this->wstrFileName, 0, sizeof(this->wstrFileName));
		memcpy(this->wstrFileName, wstrFileName, sizeof(WCHAR) * (lstrlenW(wstrFileName)));
		this->bTimeStamp = bTimeStamp;
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

BOOL LokiLog::StartLog()
{
	if (!bLogStart && bNameDefine)
	{
		pThread.bThreadStatus = TRUE;
		pThread.nOpen = 0;
		pThread.nClose = 0;
		if (bTimeStamp)
		{
			SYSTEMTIME tCurrent;
			GetSystemTime(&tCurrent);
			wsprintf(wstrFileName, TEXT("%s_%d-%d-%d-%d-%d-%d.txt"),
				wstrFileName,
				tCurrent.wYear,
				tCurrent.wMonth,
				tCurrent.wDay,
				tCurrent.wHour,
				tCurrent.wMinute,
				tCurrent.wSecond);
		}
		else
		{
			wsprintf(wstrFileName, TEXT("%s.txt"), wstrFileName);
		}
		pThread.fTarget = CreateFile(wstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		pThread.nOpen = 0;
		pThread.nClose = 0;
		memset(pThread.nBufferList, NULL, sizeof(pThread.nBufferList));
		tMain = std::thread(ThreadExecution, &pThread);
		tMain.detach();
		bLogStart = TRUE;
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

BOOL LokiLog::StopLog()
{
	if (bLogStart)
	{
		pThread.bThreadStatus = FALSE;
		bLogStart = FALSE;
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

BOOL LokiLog::ClearLog()
{

	return(FALSE);
}

BOOL LokiLog::WriteLog(LPCSTR strFormat, ...)
{
	if (pThread.bThreadStatus && bLogStart)
	{
		va_list pOperate;
		va_start(pOperate, strFormat);
		BOOL bResult = WriteToBuffer(strFormat, pOperate);
		va_end(pOperate);
		return(bResult);
	}
	else
	{
		return(FALSE);
	}
}

BOOL LokiLog::WriteToBuffer(LPCSTR strFormat, va_list arg)
{
	INT nNext = BufferStatus();
	if (nNext == -1)
	{
		return(FALSE);
	}
	vsprintf_s(pThread.nBufferList[nNext].cBuffer, MAXIMUM_BUFFER, strFormat, arg);
	pThread.nBufferList[nNext].bAvail = TRUE;
	pThread.nOpen = nNext;
	return(TRUE);
}

INT LokiLog::BufferStatus()
{
	INT nResult = (pThread.nOpen + 1) % BUFFER_LEN;
	if (pThread.nBufferList[nResult].bAvail)
	{
		return(-1);
	}
	else
	{
		return(nResult);
	}
}

void LokiLog::ThreadExecution(LokiLogThreadParam *pThread)
{
	while (pThread->nOpen != pThread->nClose || pThread->bThreadStatus)
	{
		if (pThread->nOpen != pThread->nClose)
		{
			pThread->nClose = (pThread->nClose + 1) % BUFFER_LEN;
			if (pThread->nBufferList[pThread->nClose].bAvail)
			{
				DWORD nTmp;
				WriteFile(pThread->fTarget, pThread->nBufferList[pThread->nClose].cBuffer, strlen(pThread->nBufferList[pThread->nClose].cBuffer), &nTmp, NULL);
				pThread->nBufferList[pThread->nClose].bAvail = FALSE;
			}
		}
	}
	CloseHandle(pThread->fTarget);
}
/*
int main()
{
	LokiLog *l1 = new LokiLog();
	HRC *h1 = new HRC();
	BOOL bTest = TRUE;
	bTest = bTest & l1->SetName(TEXT("log1"), TRUE);
	bTest = bTest & l1->StartLog();
	printf("%d\n", bTest);
	for (INT i1 = 0; i1 < 100000; i1++)
	{
		LLONG ltmp = h1->HighResolutionTime();
		printf("%d %ld\n", i1, ltmp);
		if (!l1->WriteLog("%d %ld\n", i1, ltmp))
		{
			break;
		}
	}
	l1->StopLog();
	system("pause");
}
*/