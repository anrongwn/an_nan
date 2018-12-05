#pragma once
#include "include\cJSON-1.7.8\cJSON.h"

/**√¸¡Ó«Î«Û°¢√¸¡ÓœÏ”¶
cmd:wfsopen/wfsclose/wfsgetinf/wfsexecute
timestamp:Now()
timeout:
data:{...}
result:{...}
*/

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

	char * getCmd();
	int getTimeStamp();
	int getTimeOut();

	int getCmdId();
	char * getServiceName();
	cJSON * getCmdParamObject();
	char * getCmdParam(); 
	void freeCmdParam(char * param) {
		cJSON_free(param);
	}

	cJSON * addResult(const char * result);
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

