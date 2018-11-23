#pragma once
#include "libuv\uv.h"

class anRun
{
public:
	anRun();
	~anRun();
public:
	static HWND hWnd_;
	static WNDCLASS wc_;
	static uv_thread_t thread_id_;
	static uv_loop_t * loop_;

	static LRESULT CALLBACK mainWndProc(
		HWND hwnd,        // handle to window
		UINT uMsg,        // message identifier
		WPARAM wParam,    // first message parameter
		LPARAM lParam);   // second message parameter
	
	static uv_loop_t * getUvLoop();
	static int start(HINSTANCE hInstance);
	static int stop();
private:
	static int init(HINSTANCE hInstance);
	static void eLoop(void * arg);
};

