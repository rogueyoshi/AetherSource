#pragma once

#include "StringHelper.h"

std::wstring to_wstring(std::string str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
}

std::string to_string(std::wstring str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(str);
}
