#pragma once
#include "libuv\\uv.h"
#include <string>

//ÄÚ´æ·ÖÅäÆ÷
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

class anRun2
{
public:
	anRun2();
	~anRun2();

public:
	static LRESULT CALLBACK mainWndProc(
		HWND hwnd,        // handle to window
		UINT uMsg,        // message identifier
		WPARAM wParam,    // first message parameter
		LPARAM lParam);   // second message parameter

	uv_loop_t * getUvLoop();
	int start(HINSTANCE hInstance);
	int stop();
	HANDLE setStdOut(HANDLE out) {
		stdout_ = out;
		return stdout_;
	};

	int sendCmd(std::string &&cmd);
private:
	HWND hWnd_;
	WNDCLASS wc_;
	uv_thread_t msg_thread_;
	uv_loop_t * loop_;
	//uv_tty_t stdin_;
	//uv_tty_t stdout_;
	HINSTANCE hInstance_;
	HANDLE stdout_;
	DWORD msg_thread_id_;

	int init(HINSTANCE hInstance);
	static void eLoop(void * arg);

	static void an_async_cb(uv_async_t* handle);
	static void an_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
	static void an_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
	static void an_close_cb(uv_handle_t* handle);
};

