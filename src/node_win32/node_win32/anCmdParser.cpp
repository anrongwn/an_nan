#include "stdafx.h"
#include "anCmdParser.h"
#include <chrono>

anCmdParser::anCmdParser(const char * cmd)
{
	root_ = cJSON_Parse(cmd);
}


anCmdParser::~anCmdParser()
{
	releaseRoot();
}

int anCmdParser::getCmdType() {
	char * cmd = nullptr;
	if (root_) {
		cJSON * item = cJSON_GetObjectItem(root_, "cmd");
		if (nullptr != item) {
			cmd = cJSON_GetStringValue(item);
		}
	}
	return (an_get_cmdtype(cmd));
}

double anCmdParser::getTimeStamp() {
	double r = 0;

	if (root_) {
		cJSON * item = cJSON_GetObjectItem(root_, "timestamp");
		if (nullptr != item) {
			r = item->valuedouble;
		}
	}

	return r;

}
int anCmdParser::getTimeOut() {
	int r = 0;

	if (root_) {
		cJSON * item = cJSON_GetObjectItem(root_, "timeout");
		if (nullptr != item) {
			r = item->valueint;
		}
	}

	return r;

}


int anCmdParser::getCmdId() {
	int cmdid = 0;
	cJSON * sub = this->getData();
	if (sub) {
		cJSON * item = cJSON_GetObjectItem(sub, "cmdid");
		if (item) {
			cmdid = item->valueint;
		}
	}

	return cmdid;
}
char * anCmdParser::getServiceName() {
	char * name = nullptr;
	cJSON * sub = this->getData();
	if (sub) {
		cJSON * item = cJSON_GetObjectItem(sub, "servicename");
		if ((item) && (false==cJSON_IsArray(item))){
			name = item->valuestring;
		}
	}

	return name;
}
int anCmdParser::getServiceNameV(std::vector<std::string> &v) {
	int num = 0;
	cJSON * sub = this->getData();
	if (sub) {
		cJSON * item = cJSON_GetObjectItem(sub, "servicename");
		if ((item)&& (cJSON_IsArray(item))) {
			num = cJSON_GetArraySize(item);
			cJSON * ai = nullptr;
			for (auto i = 0; i < num; ++i) {
				ai = cJSON_GetArrayItem(item, i);
				if (ai) {
					v.emplace_back(ai->valuestring);
				}
			}
		}
	}
	return num;
}

cJSON * anCmdParser::getCmdParamObject() {
	cJSON * param = nullptr;

	cJSON * sub = this->getData();
	if (sub) {
		param = cJSON_GetObjectItem(sub, "param");
	}

	return param;
}

char * anCmdParser::getCmdParam() {
	cJSON * param = getCmdParamObject();
	
	return cJSON_PrintUnformatted(param);//需手动释放
}

cJSON * anCmdParser::addCmdResp(const char * resp) {
	return cJSON_AddRawToObject(root_, "response", resp);
}

anResultParser::anResultParser() {
	root_ = cJSON_CreateObject();

	body_ = cJSON_CreateArray();

	cJSON_AddItemToObject(root_, "result", body_);
}

anResultParser::~anResultParser() {
	if (root_) {
		cJSON_Delete(root_);
	}
}

//reqid:0, tstimestamp:0, cmdid:0, hr:0, buffer:
anResultParser::rp_item anResultParser::insertItem() {
	rp_item item = cJSON_CreateObject();

	cJSON_AddItemToArray(body_, item);
	return item;
}
anResultParser::rp_item anResultParser::addReqId(rp_item item, double reqid) {
	return cJSON_AddNumberToObject(item, "reqid", reqid);
}

anResultParser::rp_item anResultParser::addTsTimeStamp(rp_item item) {
	//定义毫秒级别的时钟类型   
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> microClock_type;
	//获取当前时间点，windows system_clock是100纳秒级别的(不同系统不一样，自己按照介绍的方法测试)，所以要转换   
	microClock_type tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());

	return cJSON_AddNumberToObject(item, "tstimestamp", \
		tp.time_since_epoch().count());
}
anResultParser::rp_item anResultParser::addCmdId(rp_item item, int cmdid) {
	return cJSON_AddNumberToObject(item, "cmdid", cmdid);
}
anResultParser::rp_item anResultParser::addHr(rp_item item, HRESULT hr) {
	return cJSON_AddNumberToObject(item, "hr", hr);
}
anResultParser::rp_item anResultParser::addBuffer(rp_item item, const char* buffer) {
	return cJSON_AddRawToObject(item, "buffer", buffer);
}

char * anResultParser::get() {
	return cJSON_PrintUnformatted(root_);
}