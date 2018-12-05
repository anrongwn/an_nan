#include "stdafx.h"
#include "anXfsApp.h"

#include "anCmdParser.h"

#pragma comment(lib, "msxfs.lib")
#pragma comment(lib, "xfs_supp.lib")

std::atomic_bool anXfsApp::s_once_{ false };
HAPP anXfsApp::s_app_{ nullptr };

anXfsApp::anXfsApp()
{
}


anXfsApp::~anXfsApp()
{
}

HRESULT anXfsApp::initXFS() {
	HRESULT hr = WFS_SUCCESS;
	if (false == s_once_.exchange(true)) {
		WFSVERSION xfsversion = {};
		 hr = WFSStartUp(0x00030003, &xfsversion);
		 if (WFS_SUCCESS == hr) {
			 hr = WFSCreateAppHandle(&s_app_);
		 }
	}

	return hr;
}

HRESULT anXfsApp::uninitXFS() {
	HRESULT hr = WFS_SUCCESS;

	if (s_once_.exchange(false)) {
		hr = WFSDestroyAppHandle(s_app_);

		hr = WFSCleanUp();
	}
	return hr;
}

int anXfsApp::task(const char * cmd, size_t len) {
	int r = 0;

	return r;
}

void anXfsApp::insertSp(std::string& serviceName, HSERVICE hService) {
	std::lock_guard<std::mutex> lock(mtx_);

	sp_map_.emplace(serviceName, hService);
}
HSERVICE anXfsApp::getSp(const std::string& serviceName) {
	std::lock_guard<std::mutex> lock(mtx_);
	auto it = sp_map_.find(serviceName);
	if (it == sp_map_.end()) {
		return 0;
	}
	else
	{
		return it->second;
	}
}

std::string anXfsApp::getServiceName(const HSERVICE hService) {
	std::lock_guard<std::mutex> lock(mtx_);
	std::string serviceName{ "" };

	auto f = [hService](std::unordered_map<std::string, HSERVICE>::reference &ref)->bool {
		if (ref.second == hService) {
			return true;
		}
		else {
			return false;
		}
	};

	auto it = std::find_if(sp_map_.begin(), sp_map_.end(), f);
	if (it != sp_map_.end()) {
		
		serviceName=it->first;
	}

	return serviceName;
}
/**命令请求、命令响应
cmd:wfsopen/wfsclose/wfsgetinf/wfsexecute
timestamp:Now()
timeout:
data:{...}
result:{...}
*/
void anXfsApp::work_cb(uv_work_t* req) {
	an_work_req * work = static_cast<an_work_req*>(req);
	uv_buf_t * cmd = static_cast<uv_buf_t *>(work->data);

	//构造cmd
	anCmdParser cmdObject(cmd->base);
	

}

void anXfsApp::completed_work_cb(uv_work_t* req, int status) {
	an_work_req * work = static_cast<an_work_req*>(req);

	//
	work->that_->fn_(work->result_->base, work->result_->len);

	//free
	CanAllocator::an_free(work->result_->base);
	CanAllocator::an_free(work->result_);
	CanAllocator::an_free(work);
}