#include "stdafx.h"
#include "anRun.h"

#ifdef DEBUG
#pragma comment(lib, ".//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, ".//lib//libuv//release//libuv.lib")
#endif // DEBUG


const char * an_class_name = "an_node_win32";
HWND anRun::hWnd_ = nullptr;
WNDCLASS anRun::wc_ = {0};
uv_thread_t anRun::thread_id_ = { 0 };
uv_loop_t * anRun::loop_ = nullptr;

anRun::anRun()
{
}


anRun::~anRun()
{
}

LRESULT CALLBACK anRun::mainWndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)   // second message parameter
{
	switch (uMsg)
	{
	case WM_CREATE:
		// Initialize the window. 
		return 0;

	case WM_PAINT:
		// Paint the window's client area. 
		return 0;

	case WM_SIZE:
		// Set the size and position of the window. 
		return 0;

	case WM_DESTROY:
		// Clean up window-specific data objects. 
		return 0;

		// 
		// Process other messages. 
		// 

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int anRun::init(HINSTANCE hInstance) {
	int ret = 0;

	if (NULL == hInstance) return 1;

	anRun::wc_.style = CS_HREDRAW | CS_VREDRAW;
	anRun::wc_.lpfnWndProc = (WNDPROC)anRun::mainWndProc;
	anRun::wc_.cbClsExtra = 0;
	anRun::wc_.cbWndExtra = 0;
	anRun::wc_.hInstance = hInstance;
	anRun::wc_.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	anRun::wc_.hCursor = LoadCursor(NULL, IDC_ARROW);
	anRun::wc_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	anRun::wc_.lpszMenuName = an_class_name;
	anRun::wc_.lpszClassName = an_class_name;

	if (!::RegisterClass(&anRun::wc_)) return 2;

	//
	anRun::hWnd_ = ::CreateWindow(anRun::wc_.lpszClassName, anRun::wc_.lpszClassName, \
		WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (NULL == anRun::hWnd_) return 3;
	else {
		::ShowWindow(anRun::hWnd_, SW_HIDE);
	}


	//.
	return 0;
}

uv_loop_t * anRun::getUvLoop() {
	if (nullptr == anRun::loop_)
	{
		anRun::loop_ = uv_default_loop();
	}
	
	return anRun::loop_;
}

void anRun::eLoop(void * arg) {
	MSG msg = { 0 };
	uv_loop_t * loop = anRun::getUvLoop();
	
	BOOL more = FALSE;
	while (true) {
		//优先uv_loop
		if (uv_loop_alive(loop)) {
			more = uv_run(loop, UV_RUN_NOWAIT);
			if (more) continue;
		}

		//窗口消息
		if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			more = ::GetMessage(&msg, NULL, 0, 0);
			if (0 == more) {
				break;//reciv wm_quit message
			}
			if (-1 == more) {
				break;//error
			}

			//handle message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			::Sleep(1);//busy-waiting...
		}

	}
}
int anRun::start(HINSTANCE hInstance) {
	int res = 0;

	res = anRun::init(hInstance);
	if (res) return res;

	anRun::getUvLoop();

	//启动loop 线程
	res = uv_thread_create(&anRun::thread_id_, anRun::eLoop, nullptr);

	return res;
}

int anRun::stop() {
	return 0;
}