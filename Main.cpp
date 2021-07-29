#include <Windows.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <tchar.h>

/*
     Utils
*/
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

/*
* replace "steam_login" -> steam_login
*         "steam_pass"  -> steam_pass in 'filename' inside the current working directory
*/
void patchScript(char* steam_login, char* steam_pass, const char* appID, std::wstring filename) {
	TCHAR tempfilename[256]{ TEXT("test") };
	std::ifstream configFile{ filename };
	std::ofstream tempFile{ tempfilename };

	std::string line;
	while (std::getline(configFile, line))
	{
		int pos;
		bool written = false;
		if ((pos = line.find("steam_login")) != std::basic_string<char>::npos)
		{
			line.insert(pos, steam_login);
			line.erase(pos + strlen(steam_login), strlen("steam_login"));
			written = true;
		}
		if ((pos = line.find("steam_pass")) != std::basic_string<char>::npos)
		{
			line.insert(pos, steam_pass);
			line.erase(pos + strlen(steam_pass), strlen("steam_pass"));
			written = true;
		}
		if ((pos = line.find("app_update")) != std::basic_string<char>::npos)
		{
			line.clear();
			line.append("app_update " + std::string{ appID } + " validate");
			written = true;
		}
		if ((pos = line.find("app_run")) != std::basic_string<char>::npos)
		{
			line.clear();
			line.append("app_run " + std::string{ appID });
			written = true;
		}
		(!configFile.eof()) ? tempFile << line << '\n' : tempFile << line;
	}
	configFile.close();
	tempFile.close();

	std::ifstream updatedFile{ tempfilename };
	std::ofstream originFile{ filename };

	originFile << updatedFile.rdbuf();

	updatedFile.close();
	originFile.close();

	std::basic_string<TCHAR> abs_path{ current_dir() + L"\\" + tempfilename };

	if (!DeleteFile(abs_path.c_str())) {
		std::cout << "Unable to delete temp file, error code: " << GetLastError() << std::endl;
	}

}

int main(int argc, TCHAR* argv[])
{
	/*
	Step 0: Initialize
	* Encapsulate steam login and password to system variables
	* Patch script file with steam credentials, appID and absolute path to script file
	* Prepare fields to create process
	*/
	auto appID = "232890"; // appID: 232890 -> Stronghold Crusader 2
	std::wstring script_file = current_dir() + L"\\script.txt";

	patchScript(std::getenv("steam_login"), std::getenv("steam_pass"), appID, script_file);

	STARTUPINFO cif;
	ZeroMemory(&cif, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	std::wstring cmdParams{ L" +runscript " + script_file }; // SteamCMD params
	/*
	Step 1:
	* Create and run the process SteamCMD with our script which authorize to account, change install directory and download the game
	* Run the game and close SteamCMD
	* [TODO]: Handle exceptions and errors (e.g. wrong login/password, enter a Steam-guard, invalid path to directory, appID, 
	*                                            not enough space on the disc, downloading&validating > time-out of WaitForSingleObject and etc)
	*/
	
	if (!CreateProcess(L"c:\\steamcmd\\steamcmd.exe", (LPWSTR)cmdParams.c_str(), // note: server should has SteamCMD app in C:\ 
		NULL, NULL, FALSE, CREATE_NEW_CONSOLE | HIGH_PRIORITY_CLASS, NULL, NULL, &cif, &pi)) {
		std::cout << "Unable to create the process!" << std::endl;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, 1000 * 60 * 30); // 30 min time-out 

	DWORD exitCode;
	if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
		std::cout << "Unable to get exit code of the process: %d" << GetLastError() << std::endl;
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return 1;
	}
	std::cout << "Exit code: " << exitCode << std::endl;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

    return 0;
}