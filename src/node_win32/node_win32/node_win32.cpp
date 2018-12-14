// node_win32.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>

#include <signal.h>
#include <iostream>
#include <string>
#include <atomic>
#include <type_traits>

#include "cJSON-1.7.8\\cJSON.h"
#include "libuv\\uv.h"
//#include "anRun.h"
#include "anRun2.h"


//C++11 原生字符串（raw string literals）
const char * EXIT_CODE = R"(@EOT@EOT)";	//EOT

anRun2 g_event_enginer;
an_Async_Wrap g_cmd(&g_event_enginer);

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

	//C++11 原生字符串（raw string literals）
	//std::string tmp = R"({"cmd":"wfsopen","timestamp":1544410101421,"timeout":30604,"data":{"cmdid":0,"servicename":["CardReader","CashAcceptor","CashDispenser"],"param":{"spversionreq":"3.03"}}})";

	/*
	char * tmp = "{\"cmd\":\"wfsopen\",\"timestamp\":1544064176060,\"timeout\":0,\"data\":{\"cmdid\":0,\"servicename\":\"\",\"param\":{\"spversionreq\":\"3.03\"}},\"result\":{}}";
	g_anLog->info("test {}", tmp);
	*/

	/*
	sprintf_s(buf, "===argc=%d", argc);
	OutputDebugStringA(buf);
	*/
	g_anLog->debug("main argc num={}", argc);

	const int buf_size = 1024;
	char buf[buf_size] = { 0 };
	memset(buf, 0x00, buf_size);
	for (int i = 0; i < argc; ++i) {
		/*
		sprintf_s(buf, "===argv[%d] = %s", i, argv[i]);
		OutputDebugStringA(buf);
		memset(buf, 0x00, buf_size);
		*/
		g_anLog->debug("main argv[{}]= {}", i, argv[i]);
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
		g_anLog->info("main invalid stdin or stdout handle.exit(1)");
		exit(1);
	}
	
	//start
	//anRun::start(hInstance);
	g_event_enginer.setStdOut(hStdOut);
	g_event_enginer.start(hInstance);
	
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
		g_anLog->info("main recv message_len: ReadFile({})={}.", message_len, res);
		//second : read message data
		nreaded = 0;
		res = ::ReadFile(hStdIn, (void*)message, message_len, &nreaded, NULL);
		message[message_len] = '\0';

		/*
		sprintf_s(buf, "===%d ReadFile readed=%d, message_len=%d, message=%s\0", res, nreaded, message_len, message);
		OutputDebugStringA(buf);
		//memset(buf, 0x00, buf_size);
		*/
		g_anLog->info("main recv message: ReadFile(readed={}, message_len={}, message={})={}.",  nreaded, message_len, message, res);

		if ((FALSE == res) || (nreaded <= 0)) {
			continue;
		}

		//thirdly: echo 
		//std::string echo(message);
		//if (0 == echo.compare(EXIT_CODE))
		if (0==strcmp(EXIT_CODE, message))
		{
			//OutputDebugString("===reciv @EOT@EOT cmd, exit.\0");
			g_anLog->debug("main reciv {} cmd, exit.", EXIT_CODE);
			break;
		}
		int r = g_event_enginer.sendCmd(message, message_len);
		if (0 != r) {
			g_anLog->info("main g_event_enginer.sendCmd failed, ec={}", r);
			//OutputDebugString("===g_event_enginer.sendCmd failed.");
		}


		//end: clean
		message_len = 0;
		nreaded = 0;
	}

	g_event_enginer.stop();

	an_closeLog();

	g_anLog->debug("main node_win32 app exit.");
    return 0;
}

