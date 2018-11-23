// node_win32.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>

#include <signal.h>
#include <iostream>
#include <string>

#include "cJSON-1.7.8\\cJSON.h"
#include "libuv\\uv.h"
//#include "anRun.h"
#include "anRun2.h"

const char * EXIT_CODE = "\%EOT\%EOT";	//EOT

anRun2 g_app;

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
#ifdef DEBUG
	//
	HWND hw = GetConsoleWindow();
	if (NULL != hw) {
		ShowWindow(hw, SW_HIDE);
	}
#endif // DEBUG

	signal(SIGABRT, SignalHandler);

#ifdef NDEBUG
	if (argc == 1) return 0;
#endif // NDEBUG

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

	HANDLE hStdIn = INVALID_HANDLE_VALUE;
	HANDLE hStdOut = INVALID_HANDLE_VALUE;
	HANDLE hStdError = INVALID_HANDLE_VALUE;

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	if ((INVALID_HANDLE_VALUE == hStdIn) || (INVALID_HANDLE_VALUE == hStdOut)) {
		OutputDebugString("===invalid stdin or stdout handle.exit(1)");
		exit(1);
	}
	
	//start
	//anRun::start(hInstance);
	g_app.setStdOut(hStdOut);
	g_app.start(hInstance);
	
	//uv_async_init()
	BOOL res = TRUE;
	//HANDLE  hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	while (true)
	{
		//first: read message len
		int message_len = 0;
		DWORD nreaded = 0;
		//OVERLAPPED stOverlapped = { 0 };
		//stOverlapped.hEvent = hEvent;

		//res = ::ReadFile(hStdIn, (void*)&message_len, 4, &nreaded, &stOverlapped);
		res = ::ReadFile(hStdIn, (void*)&message_len, 4, &nreaded, NULL);
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
		res = ::ReadFile(hStdIn, (void*)message, message_len, &nreaded, NULL);
		
		sprintf_s(buf, "===%d ReadFile readed=%d, message_len=%d, message=%s", res, nreaded, message_len, message);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);

		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}

		//thirdly: echo 
		std::string echo(message);
		if (0 == echo.compare(EXIT_CODE))
		{
			OutputDebugString("===reciv \%EOT\%EOT cmd, exit.");
			break;
		}
		int r = g_app.sendCmd(std::move(echo));
		if (0 != r) {
			OutputDebugString("===g_app.sendCmd failed.");
		}

		

		/*
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
		*/

		//end: clean
		delete[] message;
		message_len = 0;

		/*
		DWORD len = 0, readed=0;
		res = ::ReadFile(hStdIn, (void*)buf, buf_size, &readed, NULL);
		if ((res == FALSE) || (readed <= 0)) {
			continue;
		}
		
		
		//res = ::ReadFile(hStdIn, (void *)buf, len, &readed, NULL);
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

	g_app.stop();
	OutputDebugString("===node_win32 exit.");

    return 0;
}

