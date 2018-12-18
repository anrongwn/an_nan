#pragma once
#include "include\cJSON-1.7.8\cJSON.h"
#include <unordered_map>
#include <string>


/**√¸¡Ó«Î«Û°¢√¸¡ÓœÏ”¶
cmd:wfsopen/wfsclose/wfsgetinf/wfsexecute
timestamp:Now()
timeout:
param:[cmdid:0,buffer:{}],...
result:[reqid:0, tstimestamp:0, cmdid:0, hr:0, buffer:{}],...
/////////////////
{
"cmd": "wfsopen",
"timestamp" : 1545115872208,
"timeout" : 1,
"param" : {
	"cmdid": 1024,
		"servicename" : ["CardReader", "CashAcceptor", "CashDispenser"],
		"buffer" : {
		"spversionreq": "3.03"
	}
},
"response": {
	"result": [{
		"reqid": 1545115872208,
			"cmdid" : 1024,
			"hr" : -17,
			"tstimestamp" : 1545115874099
	}, {
		"reqid": 1545115872208,
		"cmdid" : 1024,
		"hr" : -17,
		"tstimestamp" : 1545115874099
	}, {
		"reqid": 1545115872208,
		"cmdid" : 1024,
		"hr" : -17,
		"tstimestamp" : 1545115874100
	}]
}
}
*/

typedef enum {
	e_an_unknow=0,
	e_an_wfsopen,
	e_an_wfsclose,
	e_an_wfsgetinfo,
	e_an_wfsexecute,
	e_an_wfsundefined,
	e_an_end
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

struct NoCopyable {
protected:
	NoCopyable() = default;
	~NoCopyable() = default;

	NoCopyable(const NoCopyable&) = delete;
	NoCopyable& operator=(const NoCopyable&) = delete;
};

class anCmdParser : NoCopyable
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
	double getTimeStamp();
	int getTimeOut();

	int getCmdId();
	char * getServiceName();
	int getServiceNameV(std::vector<std::string> &v);
	cJSON * getCmdParamObject();
	char * getCmdParam();
	void freeOutput(char * param) {
		cJSON_free(param);
	}

	cJSON * addCmdResp(const char * resp);
	char * getCmdBuffer() const {
		return cJSON_PrintUnformatted(root_);
	}
	
protected:
	inline cJSON * getData() {
		cJSON * item = nullptr;

		if (root_) {
			item = cJSON_GetObjectItem(root_, "param");
		}

		return item;
	}
private:
	cJSON * root_{nullptr};
};

class anResultParser : NoCopyable {
public:
	anResultParser();
	~anResultParser();
	using rp_item = cJSON *;
	
	//reqid:0, tstimestamp:0, cmdid:0, hr:0, buffer:
	rp_item insertItem();
	rp_item addReqId(rp_item item, double reqid);
	rp_item addTsTimeStamp(rp_item item);
	rp_item addCmdId(rp_item item, int cmdid);
	rp_item addHr(rp_item item, HRESULT hr);
	rp_item addBuffer(rp_item item, const char* buffer);
	char * get();
	void freeOutput(char * param) {
		cJSON_free(param);
	}
private:
	cJSON * root_{ nullptr };
	cJSON * body_{ nullptr };
};