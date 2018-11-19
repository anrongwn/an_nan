// node_win32.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>

#include <signal.h>
#include <Windows.h>
#include <iostream>
#include <string>

void SignalHandler(int signal)
{
	if (signal == SIGTERM) {
		// SIGTERM signal handler code
		OutputDebugStringA("===recv SIGTERM signal, exit(3)");
		exit(3);
	}
	else {
		OutputDebugStringA("===recv other signal, exit(1)");
		exit(1);
	}
}


int main(int argc, char **argv)
{
	//
	HWND hw = GetConsoleWindow();
	if (NULL != hw) {
		ShowWindow(hw, SW_HIDE);
	}

	signal(SIGABRT, SignalHandler);

	if (argc == 1) return 0;

	const int buf_size = 1024;
	char buf[buf_size] = { 0 };

	sprintf_s(buf, "===argc=%d", argc);
	OutputDebugStringA(buf);
	memset(buf, 0x00, buf_size);
	for (int i = 0; i < argc; ++i) {
		sprintf_s(buf, "===argv[%d] = %s", i, argv[i]);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);
	}

	HANDLE hStdIo = INVALID_HANDLE_VALUE;
	HANDLE hStdOut = INVALID_HANDLE_VALUE;
	HANDLE hStdError = INVALID_HANDLE_VALUE;

	hStdIo = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	hStdError = ::GetStdHandle(STD_ERROR_HANDLE);

	if ((INVALID_HANDLE_VALUE == hStdIo) || (INVALID_HANDLE_VALUE == hStdOut)) {
		//exit(1);
	}
	

	BOOL res = TRUE;
	while (true)
	{
		//first: read message len
		int message_len = 0;
		DWORD nreaded = 0;
		res = ::ReadFile(hStdIo, (void*)&message_len, 4, &nreaded, NULL);
		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}
		
		sprintf_s(buf, "===recv message_len=%d", message_len);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);
		
		//second : read message data
		char * message = new char[message_len+1];
		message[message_len] = '\0';
		nreaded = 0;
		res = ::ReadFile(hStdIo, (void*)message, message_len, &nreaded, NULL);
		
		sprintf_s(buf, "===%d ReadFile readed=%d, message_len=%d, message=%s", res, nreaded, message_len, message);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);

		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}

		//thirdly: echo 
		std::string echo(message);
		echo += "+<<<echo ";
		SYSTEMTIME st = { 0x00 };
		::GetLocalTime(&st);
		char date[64] = { 0 };
		sprintf_s(date, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
			st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		echo += date;

		nreaded = 0;
		res = WriteFile(hStdOut, echo.c_str(), echo.length(), &nreaded, NULL);
		if(TRUE==res)
			FlushFileBuffers(hStdOut);
		else {
			OutputDebugStringA("===WriteFile faild.");
			break;
		}


		//end: clean
		delete[] message;
		message_len = 0;

		/*
		DWORD len = 0, readed=0;
		res = ::ReadFile(hStdIo, (void*)buf, buf_size, &readed, NULL);
		if ((res == FALSE) || (readed <= 0)) {
			continue;
		}
		
		
		//res = ::ReadFile(hStdIo, (void *)buf, len, &readed, NULL);
		if (res) {
			std::string echo(buf);
			echo += "+<<<echo ";
			SYSTEMTIME st = { 0x00 };
			::GetLocalTime(&st);
			char date[100] = { 0 };
			sprintf_s(date, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
				st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
			echo += date;

			WriteFile(hStdOut, echo.c_str(), echo.length(), &readed, NULL);

			FlushFileBuffers(hStdOut);
		}
		
		memset(buf, 0x00, buf_size);
		::Sleep(10);
		*/
	}

    return 0;
}

