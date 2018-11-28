// stdafx.cpp : 只包括标准包含文件的源文件
// node_win32.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

std::shared_ptr<spdlog::logger> g_anLog;
#define	AN_LOG_NAME	("an_node_win32")
void an_initLog() {
	//OutputDebugString("===g_anLog init start...");
	spdlog::init_thread_pool(65536, 1);
	g_anLog = spdlog::daily_logger_mt<spdlog::async_factory>(AN_LOG_NAME, "logs/an_node_win32.log");

	g_anLog->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
	g_anLog->set_level(spdlog::level::trace);
	

	//OutputDebugString("===g_anLog init");
}

void an_closeLog() {
	spdlog::drop(AN_LOG_NAME);
}