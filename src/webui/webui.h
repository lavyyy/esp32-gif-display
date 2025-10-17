// webui.h
#pragma once
#include <WebServer.h>
extern volatile bool g_need_reload, g_uploading, g_request_stop, g_swap_ready;
void webui_begin();
void webui_handle();
