// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <winsock2.h> 
#include <Windows.h>
#include <io.h>

#include "include\spdlog\spdlog.h"
#include "include\spdlog\async.h"
#include "include\spdlog\sinks\basic_file_sink.h"
#include "include\spdlog\sinks\daily_file_sink.h"
extern std::shared_ptr<spdlog::logger> g_anLog;
void an_initLog();
void an_closeLog();
