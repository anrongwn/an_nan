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


const char * EXIT_CODE = "@EOT@EOT";	//EOT

anRun2 g_app;
an_Async_Wrap g_cmd(&g_app);

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
	an_initLog();

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

	/*
	sprintf_s(buf, "===argc=%d", argc);
	OutputDebugStringA(buf);
	*/
	g_anLog->debug("===argc num={}", argc);

	memset(buf, 0x00, buf_size);
	for (int i = 0; i < argc; ++i) {
		/*
		sprintf_s(buf, "===argv[%d] = %s", i, argv[i]);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);
		*/
		g_anLog->debug("===argv[{}]= {}", i, argv[i]);
	}

	HANDLE hStdIn = INVALID_HANDLE_VALUE;
	HANDLE hStdOut = INVALID_HANDLE_VALUE;
	HANDLE hStdError = INVALID_HANDLE_VALUE;

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	hStdError = ::GetStdHandle(STD_ERROR_HANDLE);
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	if ((INVALID_HANDLE_VALUE == hStdIn) || (INVALID_HANDLE_VALUE == hStdOut)) {
		//OutputDebugString("===invalid stdin or stdout handle.exit(1)");
		g_anLog->info("===invalid stdin or stdout handle.exit(1)");
		exit(1);
	}
	
	//start
	//anRun::start(hInstance);
	g_app.setStdOut(hStdOut);
	g_app.start(hInstance);
	
	//uv_async_init()
	BOOL res = TRUE;
	//HANDLE  hEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);


	int message_len = 0;
	DWORD nreaded = 0;
	char message[65536] = { 0 };
	while (true)
	{
		//first: read message len
		res = ::ReadFile(hStdIn, (void*)&message_len, 4, &nreaded, NULL);
		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}
		
		/*
		sprintf_s(buf, "===recv message_len=%d\0", message_len);
		OutputDebugStringA(buf);
		//memset(buf, 0x00, buf_size);
		*/
		g_anLog->info("===recv message_len={}", message_len);
		//second : read message data
		nreaded = 0;
		res = ::ReadFile(hStdIn, (void*)message, message_len, &nreaded, NULL);
		message[message_len] = '\0';

		/*
		sprintf_s(buf, "===%d ReadFile readed=%d, message_len=%d, message=%s\0", res, nreaded, message_len, message);
		OutputDebugStringA(buf);
		//memset(buf, 0x00, buf_size);
		*/
		g_anLog->info("==={} ReadFile readed={}, message_len={}, message={}", res, nreaded, message_len, message);

		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}

		//thirdly: echo 
		std::string echo(message);
		if (0 == echo.compare(EXIT_CODE))
		{
			//OutputDebugString("===reciv @EOT@EOT cmd, exit.\0");
			g_anLog->debug("===reciv {} cmd, exit.", EXIT_CODE);
			break;
		}
		int r = g_app.sendCmd(std::move(echo));
		if (0 != r) {
			g_anLog->info("===g_app.sendCmd failed, ec={}", r);
			//OutputDebugString("===g_app.sendCmd failed.");
		}


		//end: clean
		message_len = 0;
		nreaded = 0;
	}

	g_app.stop();
	//OutputDebugString("===node_win32 exit.");

	g_anLog->debug("===node_win32 app exit.");

	an_closeLog();
    return 0;
}

