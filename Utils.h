#ifndef __UTILS_H__
#define __UTILS_H__

#include <Windows.h>
#include <string>

std::basic_string<TCHAR> current_dir();

std::wstring to_wstring(const char* str);

#endif