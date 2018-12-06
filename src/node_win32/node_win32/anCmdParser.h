#pragma once
#include "include\cJSON-1.7.8\cJSON.h"
#include <unordered_map>
#include <string>


/**√¸¡Ó«Î«Û°¢√¸¡ÓœÏ”¶
cmd:wfsopen/wfsclose/wfsgetinf/wfsexecute
timestamp:Now()
timeout:
data:{...}
result:{...}
*/
typedef enum {
	e_an_wfsopen=1,
	e_an_wfsclose,
	e_an_wfsgetinfo,
	e_an_wfsexecute,
	e_an_wfsundefined
}e_an_cmdtype;
static inline int an_get_cmdtype(const char*cmd) {
	static std::unordered_map<std::string, int> s_type_map = { {"wfsopen", e_an_wfsopen }, {"wfsclose", e_an_wfsclose }, \
		{"wfsgetinfo", e_an_wfsgetinfo}, {"wfsexecute", e_an_wfsexecute } };

	int type = e_an_wfsundefined;
	auto it = s_type_map.find(std::string(cmd));
	if (it != s_type_map.end()) {
		type = it->second;
	}

	return type;
}


class anCmdParser
{
public:
	anCmdParser()=delete;
	explicit anCmdParser(const char * cmd);
	~anCmdParser();

	void releaseRoot() {
		if (root_) {
			cJSON_Delete(root_);
			root_ = nullptr;
		}
	}

	int getCmdType();
	int getTimeStamp();
	int getTimeOut();

	int getCmdId();
	char * getServiceName();
	cJSON * getCmdParamObject();
	char * getCmdParam(); 
	void freeCmdParam(char * param) {
		cJSON_free(param);
	}

	cJSON * addCmdResp(const char * resp);
	char * getCmdBuffer() {
		return cJSON_Print(root_);
	}
	void freeCmdBuffer(char * buffer) {
		cJSON_free(buffer);
	}
protected:
	inline cJSON * getData() {
		cJSON * item = nullptr;

		if (root_) {
			item = cJSON_GetObjectItem(root_, "data");
		}

		return item;
	}
private:
	cJSON * root_{nullptr};
};

