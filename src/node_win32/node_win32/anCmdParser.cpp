#include "stdafx.h"
#include "anCmdParser.h"


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

int anCmdParser::getTimeStamp() {
	int r = 0;

	if (root_) {
		cJSON * item = cJSON_GetObjectItem(root_, "timestamp");
		if (nullptr != item) {
			r = item->valueint;
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
	
	return cJSON_PrintUnformatted(param);//���ֶ��ͷ�
}

cJSON * anCmdParser::addCmdResp(const char * resp) {
	return cJSON_AddRawToObject(root_, "response", resp);
}