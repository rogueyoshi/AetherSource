#pragma once


#include <codecvt>


#ifdef UNICODE
typedef std::wstring string;
typedef std::wstringstream stringstream;
void print(string str) { std::fwprintf(stdout, str.c_str()); }
#else
typedef std::string string;
typedef std::stringstream stringstream;
void print(string str) { std::fprintf(stdout, str.c_str()); }
#endif


std::string tostring(std::wstring str) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str); }
std::wstring towstring(std::string str) { return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str); }
