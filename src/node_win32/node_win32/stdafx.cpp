// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// node_win32.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H �������κ�����ĸ���ͷ�ļ���
//�������ڴ��ļ�������

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