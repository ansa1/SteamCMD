#include "Utils.h"

std::basic_string<TCHAR> current_dir() {
	TCHAR cur_path[256];
	GetCurrentDirectory(256, cur_path);
	return cur_path;
}

std::wstring to_wstring(const char* str) {
	size_t length1 = strlen(str);
	std::wstring res(length1, L'#');
	mbstowcs(&res[0], str, length1);
	return res;
}