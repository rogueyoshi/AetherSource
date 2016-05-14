#pragma once


#include <Windows.h>
#include <tchar.h>


class OpenFileDialog
{
public:
	OpenFileDialog();

	bool ShowDialog();

	TCHAR* DefaultExtension;
	TCHAR* FileName;
	TCHAR* Filter;
	int FilterIndex;
	int Flags;
	TCHAR* InitialDir;
	HWND Owner;
	TCHAR* Title;
};
