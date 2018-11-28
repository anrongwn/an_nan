#include "stdafx.h"
#include "anRun2.h"
#include <string>
#include <sstream>
#include <chrono>

#ifdef DEBUG
#pragma comment(lib, ".//lib//libuv//debug//libuv.lib")
#else
#pragma comment(lib, ".//lib//libuv//release//libuv.lib")
#endif // DEBUG

static const char * an_class_name = "an_node_win32";
extern an_Async_Wrap g_cmd;
anRun2::anRun2()
{
	hWnd_ = nullptr;
	wc_ = { 0 };
	msg_thread_ = { 0 };
	loop_ = nullptr;
	//stdin_ = { 0 };
	stdout_ = nullptr;
	hInstance_ = nullptr;

	msg_thread_id_ = 0;
}


anRun2::~anRun2()
{
}

LRESULT CALLBACK anRun2::mainWndProc(
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

int anRun2::init(HINSTANCE hInstance) {
	int ret = 0;

	if (NULL == hInstance) return 1;

	this->hInstance_ = hInstance;
	this->wc_.style = CS_HREDRAW | CS_VREDRAW;
	this->wc_.lpfnWndProc = (WNDPROC)anRun2::mainWndProc;
	this->wc_.cbClsExtra = 0;
	this->wc_.cbWndExtra = 0;
	this->wc_.hInstance = hInstance;
	this->wc_.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	this->wc_.hCursor = LoadCursor(NULL, IDC_ARROW);
	this->wc_.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	this->wc_.lpszMenuName = an_class_name;
	this->wc_.lpszClassName = an_class_name;

	if (!::RegisterClass(&this->wc_)) return 2;

	//
	this->hWnd_ = ::CreateWindow(this->wc_.lpszClassName, this->wc_.lpszClassName, \
		WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (NULL == this->hWnd_) return 3;
	else {
		::ShowWindow(this->hWnd_, SW_HIDE);
	}

	/*//
	getUvLoop();

	HANDLE hStdIo = INVALID_HANDLE_VALUE;
	HANDLE hStdOut = INVALID_HANDLE_VALUE;

	hStdIo = GetStdHandle(STD_INPUT_HANDLE);
	hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
	

	int ttyin_fd=-1, ttyout_fd=-1;
	ttyin_fd = _open_osfhandle((intptr_t)hStdIo, _O_TEXT);
	ttyout_fd = _open_osfhandle((intptr_t)hStdOut, _O_TEXT);
	int tty1 = uv_open_osfhandle(hStdIo);
	int tty2 = uv_open_osfhandle(hStdOut);

	
	uv_handle_type tt;
	tt = uv_guess_handle(ttyin_fd);

	tt = uv_guess_handle(ttyout_fd);

	ret = uv_tty_init(loop_, &stdin_, ttyin_fd, 1);
	uv_handle_set_data((uv_handle_t *)&stdin_, this);

	//ret = uv_tty_set_mode(&stdin_, UV_TTY_MODE_RAW);

	ret = uv_tty_init(loop_, &stdout_, ttyout_fd, 0);

	int n = uv_is_readable((uv_stream_t*)&stdin_);
	n = uv_is_writable((uv_stream_t*)&stdout_);
	*/

	return 0;
}

uv_loop_t * anRun2::getUvLoop() {
	if (nullptr == this->loop_)
	{
		this->loop_ = uv_default_loop();
	}

	return this->loop_;
}

void anRun2::eLoop(void * arg) {

	anRun2 * that = static_cast<anRun2 *>(arg);
	MSG msg = { 0 };
	uv_loop_t * loop = that->getUvLoop();

	that->msg_thread_id_ = ::GetCurrentThreadId();

	BOOL more = FALSE;
	while (true) {
		//����uv_loop
		if (uv_loop_alive(loop)) {
			//OutputDebugString("====uv_loop_alive is true.");
			//more = uv_run(loop, UV_RUN_NOWAIT);
			more = uv_run(loop, UV_RUN_ONCE);
			if (more) continue;
		}

		//������Ϣ
		//OutputDebugString("====PeekMessage...");
		if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			more = ::GetMessage(&msg, NULL, 0, 0);
			if (0 == more) {
				//OutputDebugString("====GetMessage wm_quit message...");
				g_anLog->info("===GetMessage wm_quit message...");
				break;//reciv wm_quit message
			}
			if (-1 == more) {
				//OutputDebugString("====GetMessage -1 error.");
				g_anLog->info("===GetMessage -1 error.");
				break;//error
			}

			//handle message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			//OutputDebugString("====GetMessage msg.");
			g_anLog->info("===DispatchMessage {} msg", msg.lParam);
		}
		else {
			::Sleep(1);//busy-waiting...
		}

	}

	//OutputDebugString("====eLoop thread exit.");
	g_anLog->info("===eLoop thread exit.");
}
int anRun2::start(HINSTANCE hInstance) {
	int res = 0;

	res = this->init(hInstance);
	if (res) return res;

	this->getUvLoop();

	//����loop �߳�
	res = uv_thread_create(&this->msg_thread_, this->eLoop, this);

	/*
	//����stdio ��
	res = uv_read_start((uv_stream_t*)&this->stdin_, anRun2::an_alloc_cb, anRun2::an_read_cb);
	std::stringstream ss;
	ss << "===uv_read_start=" << res;
	OutputDebugString(ss.str().c_str());

	//anRun2::eLoop(this);
	*/
	return res;
}

int anRun2::stop() {
	BOOL r = FALSE;
	r = ::PostThreadMessage(this->msg_thread_id_, WM_QUIT, 0, 0);
	if (FALSE==r) {

		/*
		std::stringstream ss;
		ss << this->msg_thread_id_ << " PostThreadMessage failed." << "rc=" << r << "ec=" << GetLastError();
		OutputDebugString(ss.str().c_str());
		*/

		g_anLog->debug("{} PostThreadMessage failed, rc={}, ec={}", this->msg_thread_id_, r, GetLastError());
	}

	uv_thread_join(&this->msg_thread_);

	return 0;
}
void anRun2::an_close_cb(uv_handle_t* handle) {
	if (handle->type == UV_ASYNC) {
		g_cmd.push((an_Async*)handle);

		uv_unref(handle);
	}
	//CanAllocator::an_free(handle);
}

static SYSTEMTIME st = { 0x00 };
static char cmd_data[65536] = { 0 };
static int cmd_len = 0;
static const char * echo_flag = "+<<<echo ";
static char date_tmp[24] = { 0 };

void anRun2::an_async_cb(uv_async_t* handle) {
	an_Async * p_as_data = static_cast<an_Async*>(handle);

	auto start = std::chrono::system_clock::now();

	/*
	std::string echo(p_as_data->base, p_as_data->len);
	echo += "+<<<echo ";
	::GetLocalTime(&st);
	sprintf_s(date, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
		st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	echo += date;
	*/

	cmd_len = p_as_data->len;
	memcpy(cmd_data, p_as_data->base, cmd_len);
	memcpy(cmd_data + cmd_len, echo_flag, 9);
	cmd_len += 9;
	::GetLocalTime(&st);
	sprintf_s(date_tmp, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, \
		st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	memcpy(cmd_data + cmd_len, date_tmp, 24);
	cmd_len += 24;
	
	
	
	//OutputDebugStringA(echo.c_str());
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	g_anLog->info("===echo Res={}, et={:f}", cmd_data, elapsed_seconds.count());
	DWORD nreaded = 0;
	BOOL res = WriteFile(((anRun2*)(p_as_data->that))->stdout_, cmd_data, cmd_len, &nreaded, NULL);

	/*
	g_anLog->info("===echo Res={}, et={:f}", echo, elapsed_seconds.count());
	DWORD nreaded = 0;
	BOOL res = WriteFile(((anRun2*)(p_as_data->that))->stdout_, echo.c_str(), echo.length(), &nreaded, NULL);
	*/
	if (TRUE == res)
		FlushFileBuffers(((anRun2*)(p_as_data->that))->stdout_);
	else {
		//OutputDebugStringA("===WriteFile faild.");
		g_anLog->info("===WriteFile faild, ec={}", ::GetLastError());
	}

	//CanAllocator::an_free(p_as_data->base);
	//CanAllocator::an_free(p_as_data);

	uv_close((uv_handle_t*)handle, anRun2::an_close_cb);
}
int anRun2::sendCmd(const char * cmd, size_t len) {
	/*
	uv_async_t * as = (uv_async_t*)CanAllocator::an_malloc(sizeof(uv_async_t));
	anCmd * p_as_data = (anCmd*)CanAllocator::an_malloc(sizeof(anCmd));
	p_as_data->len = cmd.length();
	p_as_data->base = CanAllocator::an_malloc(p_as_data->len);
	memcpy(p_as_data->base, cmd.c_str(), p_as_data->len);
	p_as_data->that = this;

	as->data = p_as_data;
	*/
	an_Async * as = g_cmd.pop();
	if (as) {
		as->setCmd(cmd, len);
		/*
		std::stringstream ss;
		ss << "===as " << as->base << ", len=" << as->len;
		OutputDebugString(ss.str().c_str());
		*/
	}
	else {
		/*
		std::stringstream ss;
		ss << "===as is nullptr";
		OutputDebugString(ss.str().c_str());
		*/

		g_anLog->debug("===an_Async * as = g_cmd.pop() is nullptr.");
	}
	

	int r = 0;
	r = uv_async_init(loop_, as, anRun2::an_async_cb);
	if (0 != r) {
		/*
		std::stringstream ss;
		ss << "===uv_async_init failed.ec=" << r << ", " << uv_strerror(r);
		OutputDebugString(ss.str().c_str());
		*/
		g_anLog->debug("===uv_async_init failed, ec={}, error={}", r, uv_strerror(r));
	}

	r = uv_async_send(as);
	if (0 != r) {
		/*
		CanAllocator::an_free(p_as_data->base);
		CanAllocator::an_free(p_as_data);
		CanAllocator::an_free(as);
		*/
		/*
		std::stringstream ss;
		ss << "===uv_async_send failed.ec=" << r << ", " << uv_strerror(r);
		OutputDebugString(ss.str().c_str());
		*/
		g_anLog->debug("===uv_async_send failed, ec={}, error={}", r, uv_strerror(r));
	}

	return r;
}

void anRun2::an_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = CanAllocator::an_malloc(suggested_size);
	buf->len = suggested_size;
}

void anRun2::an_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
	anRun2 * that = static_cast<anRun2*>(uv_handle_get_data((uv_handle_t*)stream));

	std::stringstream out;
	out << "===an_read_cb(nread=" << nread << ")";
	OutputDebugString(out.str().c_str());

	out.str("");
	if (nread > 0) {
		std::string log(buf->base, nread);
		
		out << "===reciv data: " << log;

		OutputDebugString(out.str().c_str());

		

	}
	else if (nread < 0) {
		if (UV_EOF == nread) {

		}

		OutputDebugString(uv_strerror(nread));
	}
}