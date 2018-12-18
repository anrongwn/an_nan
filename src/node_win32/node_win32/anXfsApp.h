#pragma once
#include <XFSAPI.H>
#include <XFSIDC.H>
#include "an_async.h"
#include <atomic>
#include <mutex>
#include <functional>
#include <string>
#include <unordered_map>
#include <map>
#include <iterator>
#include <memory>
#include <utility>

#include "anCmdParser.h"

//
class anXfsApp
{
	/*
	template<typename T, typename... Args>
	using Constructor = std::function<std::shared_ptr<T>(Args...)>;
	*/

public:
	anXfsApp();
	~anXfsApp();

	//注册完成消息回调函数
	template<class F, typename T, class... Args>
	void register_completed_cb(F&& f, T&& obj, Args&&... args) {
		fn_ = std::bind(std::forward<F>(f), std::forward<T>(obj), \
			std::placeholders::_1, std::placeholders::_2);

		/*
		auto name = typeid(Constructor<T, Args...>).name();
		OutputDebugString(name);
		*/

		//启动xfs
		if (fn_)
			anXfsApp::initXFS();

	};

	//
	

	//
	void insertSp(std::string& serviceName, HSERVICE hService);
	HSERVICE getSp(const std::string& serviceName);
	std::string getServiceName(const HSERVICE hService);
public:
	static HRESULT initXFS();
	static HRESULT uninitXFS();

	static void work_cb(uv_work_t* req);
	static void completed_work_cb(uv_work_t* req, int status);

	static HRESULT an_wfsopen(anXfsApp* that, anCmdParser *cmd, char** result);

private:
	static std::atomic_bool s_once_;
	static HAPP s_app_;

	std::mutex mtx_{};
	std::function<int(an_work_req *, int)> fn_;
	std::unordered_map<std::string, HSERVICE> sp_map_;//service_name=hservice

};

