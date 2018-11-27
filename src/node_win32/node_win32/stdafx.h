// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  在此处引用程序需要的其他头文件
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
