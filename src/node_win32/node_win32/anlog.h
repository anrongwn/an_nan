#pragma once

#include <string>
#include <ostream>

#include "include\spdlog\spdlog.h"
#include "include\spdlog\async.h"
#include "include\spdlog\sinks\basic_file_sink.h"
#include "include\spdlog\sinks\daily_file_sink.h"

static std::shared_ptr<spdlog::logger> g_anLog;

void an_initLog() {
	spdlog::init_thread_pool(65536, 1);
	g_anLog = spdlog::daily_logger_mt<spdlog::async_factory>("async_file_logger", "logs/anlog.log");

	g_anLog->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	g_anLog->set_level(spdlog::level::trace);
}

void an_log()