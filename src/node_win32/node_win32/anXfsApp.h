#pragma once
#include <XFSAPI.H>
#include <XFSIDC.H>
#include "an_async.h"
#include <atomic>
#include <mutex>
#include <functional>
#include <string>
#include <unordered_map>




class anXfsApp
{
public:
	anXfsApp();
	~anXfsApp();

	//注册完成消息回调函数
	template<class F, typename T, class... Args>
	void register_completed_cb(F&& f, T&& obj, Args&&... args) {
		fn_ = std::bind(std::forward<F>(f), std::forward<T>(obj), \
			std::placeholders::_1, std::placeholders::_2);
	};

	//
	int task(const char *cmd, size_t len);

	//
	void insertSp(std::string& serviceName, HSERVICE hService);
	HSERVICE getSp(const std::string& serviceName);
	std::string getServiceName(const HSERVICE hService);
public:
	static HRESULT initXFS();
	static HRESULT uninitXFS();

	static void work_cb(uv_work_t* req);
	static void completed_work_cb(uv_work_t* req, int status);

private:
	static std::atomic_bool s_once_;
	static HAPP s_app_;

	std::mutex mtx_{};
	std::function<int(const char*, size_t)> fn_;
	std::unordered_map<std::string, HSERVICE> sp_map_;//service_name=hservice
};

