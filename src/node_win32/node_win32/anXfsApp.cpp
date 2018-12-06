#include "stdafx.h"
#include "anXfsApp.h"

#include "anCmdParser.h"
#include <ctime>

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

	//g_anLog->info("anXfsApp::work_cb cmd={}, len={}", cmd->base, cmd->len);
	//OutputDebugString("=== anXfsApp::work_cb begin...");

	//构造cmd
	anCmdParser cmdObject(cmd->base);
	int cmdtype = cmdObject.getCmdType();
	int timestamp = cmdObject.getTimeStamp();
	int timeout = cmdObject.getTimeOut();
	char * servicename = cmdObject.getServiceName();
	int cmdid = cmdObject.getCmdId();
	char * cmdParam = cmdObject.getCmdParam();

	

	switch (cmdtype) {
	case e_an_wfsopen:
		break;
	case e_an_wfsclose:
		break;
	case e_an_wfsgetinfo:
		break;
	case e_an_wfsexecute:
		break;
	default:
		break;
	}
	char * tmp = "{\"rc\":1054,\"result\":{\"hr\":1054}}";
	cmdObject.addCmdResp(tmp);

	char *cmdresp = cmdObject.getCmdBuffer();
	g_anLog->info("anXfsApp::work_cb josn cmd={}", cmdresp);

	/*
	//echo 
	uv_buf_t * cmd_buffer = (uv_buf_t *)work->data;
	const char * echo_flag = "+<<<echo ";
	int cmd_len = cmd_buffer->len;
	static char cmd_data[65536];
	memcpy(cmd_data, cmd_buffer->base, cmd_len);
	memcpy(cmd_data + cmd_len, echo_flag, 9);
	cmd_len += 9;

	SYSTEMTIME st = { 0x00 };
	::GetLocalTime(&st);
	char date_tmp[24] = { 0 };
	sprintf_s(date_tmp, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
		st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	memcpy(cmd_data + cmd_len, date_tmp, 24);
	cmd_len += 24;
	//cmd_data[cmd_len] = '\0';
	*/

	uv_buf_t * resp = (uv_buf_t *)CanAllocator::an_malloc(sizeof(uv_buf_t));
	/*
	tmp->base = CanAllocator::an_malloc(cmd_len);
	memcpy(tmp->base, cmd_data, cmd_len);
	tmp->len = cmd_len;
	*/
	int cmd_len = strlen(cmdresp);
	resp->base = CanAllocator::an_malloc(cmd_len);
	memcpy(resp->base, cmdresp, cmd_len);
	resp->len = cmd_len;

	work->resp_ = resp;
	
	//OutputDebugString("=== anXfsApp::work_cb end.");
}

void anXfsApp::completed_work_cb(uv_work_t* req, int status) {
	an_work_req * work = static_cast<an_work_req*>(req);

	//
	int r = work->that_->fn_(work->resp_->base, work->resp_->len);
		
	//free
	CanAllocator::an_free(work->resp_->base);
	CanAllocator::an_free(work->resp_);
	CanAllocator::an_free(((uv_buf_t*)(work->data))->base);
	CanAllocator::an_free(work->data);
	CanAllocator::an_free(work);

}