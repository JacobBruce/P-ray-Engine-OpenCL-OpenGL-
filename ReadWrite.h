#pragma once
#include <string>
#include <fstream>
#include <assert.h>
#include "Resource.h"
#include "MathExt.h"

using namespace std;

static string ErrorCodeToStr(const int ecode) 
{
	string emsg;
	switch (ecode)
	{
	case 060:
		emsg = "Cannot locate font file:\n";
		break;
	case 130:
		emsg = "Cannot locate mesh file:\n";
		break;
	case 110:
		emsg = "Cannot locate material definition file:\n";
		break;
	case 150:
		emsg = "Cannot locate texture map file:\n";
		break;
	case 151:
		emsg = "Invalid LoD for texture map file:\n";
		break;
	case 200:
		emsg = "Cannot locate texture file:\n";
		break;
	default:
		emsg = "n/a";
		break;
	}
	return emsg;
}

static void HandleFatalError(const int ecode, string emsg)
{
	wstring wemsg = StrToWstr(emsg);
	HWND wHandle = GetForegroundWindow();

    int msgboxID = MessageBox(
        wHandle,
        (LPCWSTR)(L"The application encountered a fatal error and needs to close.\n\nError message: "+wemsg).c_str(),
        (LPCWSTR)(L"Fatal Error ("+IntToWstr(ecode)+L")").c_str(), MB_ICONERROR | MB_OK | MB_SYSTEMMODAL | MB_SETFOREGROUND
    );

	exit(EXIT_FAILURE);
}

static string readFile(const string filename) 
{
	ifstream sourceFile(filename);
    string sourceCode(istreambuf_iterator<char>(sourceFile),(istreambuf_iterator<char>()));
	return sourceCode;
}

static void LoadConfigFile(const string filename)
{
	ifstream levelfile(filename);
	string line, key, data;
	size_t bpos;

	if (levelfile.is_open()) {

		while (!levelfile.eof()) {
			getline(levelfile, line);
			bpos = line.find("=");
			if (bpos == string::npos) { continue; }
			key = line.substr(0, bpos);
			data = line.substr(bpos+1);
			GLOBALS::config_map[key] = data;
		}

		levelfile.close();
	}
}

static void CLBLog(const string logstr) 
{
	if (CL_COMPLOG) {
		ofstream clfile("logs\\cl_cout.log", ofstream::app);
		if (clfile.is_open()) {
			clfile << logstr.c_str() << "\n";
			clfile.close();
		}
	}
}