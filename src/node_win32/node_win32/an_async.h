#pragma once
#include "libuv\uv.h"
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include <algorithm>

class anRun2;

//内存分配器
class CanAllocator {
public:
	static char * an_malloc(size_t size) {
		return (char*)malloc(size);
	}

	static void an_free(void *buf) {
		free(buf);
	}
};

struct anCmd : public uv_buf_t {
	void * that;
};

struct an_Async : public uv_async_t, anCmd {
	an_Async(anRun2 * v) {
		this->that = v;
		this->data = nullptr;
	}

	size_t alloc_Cmd(size_t len) {
		this->base = CanAllocator::an_malloc(len);
		this->len = len;
		this->data = this->base;
		return this->len;
	}

	void setCmd(const char *data, size_t len) {
		/*
		std::stringstream ss;
		ss << "===setcmd(" << data << ", len=" << len;
		*/
		
		memcpy(this->base, data, len);

		this->data = this->base;
		this->len = len;
		
	}
	~an_Async() {
		if (this->base) {
			CanAllocator::an_free(this->base);
			this->len = 0;
		}

		//OutputDebugStringA("===~an_Async()");
	}
	
};

#define AN_ASYNC_SIZE (1000)
struct an_Async_Wrap {
	an_Async_Wrap(anRun2* that):that_(that) {
		uv_mutex_init(&mx_);

		uv_mutex_lock(&mx_);
		/*
		for (int i = 0; i < AN_ASYNC_SIZE; ++i) {
			an_Async * p = new an_Async(that_);
			p->alloc_Cmd(65536);

			async_queue_.push(p);
		}
		*/
		this->pre_alloc(AN_ASYNC_SIZE);

		uv_mutex_unlock(&mx_);
	}

	size_t pre_alloc(size_t n) {
		for (int i = 0; i < n; ++i) {
			an_Async * p = new an_Async(that_);
			p->alloc_Cmd(65536);

			async_queue_.push(p);
		}

		return async_queue_.size();
	}
	an_Async * pop() {
		an_Async * p = nullptr;
		uv_mutex_lock(&mx_);
		if (!async_queue_.empty())
		{
			p = async_queue_.front();
			async_queue_.pop();
		}
		
		/*//不二次分配
		if (nullptr == p) {
			if (this->pre_alloc(100)) {
				p = async_queue_.front();
				async_queue_.pop();
			}
		}
		*/

		uv_mutex_unlock(&mx_);

		/*
		if (p == nullptr) {
			OutputDebugStringA("===pop null.");
		}
		*/
		return p;
	}
	void push(an_Async *cmd) {
		uv_mutex_lock(&mx_);
		//async_queue_.push(cmd);
		async_queue_.emplace(cmd);
		uv_mutex_unlock(&mx_);
	}

	~an_Async_Wrap() {
		uv_mutex_lock(&mx_);
		an_Async * p = nullptr;
		while (!async_queue_.empty()) {
			p = async_queue_.front();
			async_queue_.pop();
			delete p;
			p = nullptr;
		}
		uv_mutex_unlock(&mx_);
		//OutputDebugStringA("===~an_Async_Wrap() finished.");
	}
private:
	uv_mutex_t mx_;
	std::queue<an_Async*> async_queue_;
	class anRun2 * that_;
};

//
struct an_work_req : public uv_work_t {
	class anXfsApp * that_;
	uv_buf_t * resp_;	//命令返回结果buf

	an_work_req() = delete;
	explicit an_work_req(class anXfsApp *handle) {
		data = nullptr;
		that_ = handle;
		resp_ = nullptr;
	}

	~an_work_req() {
		uv_buf_t * tmp = static_cast<uv_buf_t*>(data);
		if (tmp) {
			if (tmp->base) {
				CanAllocator::an_free(tmp->base);
				//tmp->len = 0;
			}
			CanAllocator::an_free(data);
		}
		if (resp_) {
			if (resp_->base) {
				CanAllocator::an_free(resp_->base);
				//resp_->len = 0;
			}
			CanAllocator::an_free(resp_);
		}
	}

	/*//注意只设置一次，多次造成内存泄漏*/
	uv_buf_t * setData(const char *cmd, size_t len){
		uv_buf_t * cmd_data = (uv_buf_t*)CanAllocator::an_malloc(sizeof(uv_buf_t));
		cmd_data->len = (len + 1);
		cmd_data->base = CanAllocator::an_malloc(cmd_data->len);
		memcpy(cmd_data->base, cmd, len);
		cmd_data->base[len] = '\0';

		data = cmd_data;
		return cmd_data;
	}
	/*//注意只设置一次，多次造成内存泄漏*/
	uv_buf_t * setResp(const char *buf, size_t len) {
		resp_ = (uv_buf_t *)CanAllocator::an_malloc(sizeof(uv_buf_t));
		resp_->len = len + 1;
		resp_->base = CanAllocator::an_malloc(resp_->len);
		memcpy(resp_->base, buf, resp_->len);
		resp_->base[len] = '\0';

		return resp_;
	}
};
