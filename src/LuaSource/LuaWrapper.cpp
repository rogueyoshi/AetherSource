#include <algorithm>
#define NOMINMAX

#include "LuaWrapper.h"
#include "StringHelper.h"

#define MINIMUM_FPS 6

// Wrap the lua function wrapper into a namespace for disambiguation and future-proofing in case we add more lua classes.
namespace NLuaWrapper
{
	typedef int (CLuaWrapper::*signature)(lua_State *L);

	// This template wraps a member function into a C-style "free" function compatible with lua.
	template <signature function>

	int dispatch(lua_State *L)
	{
		CLuaWrapper *pointer = *static_cast<CLuaWrapper**>(lua_getextraspace(L));

		return ((*pointer).*function)(L);
	}
}

CLuaWrapper::CLuaWrapper() :
	m_iFPS(MINIMUM_FPS),
	m_pLuaState(NULL)
{
	m_pDirectXWrapper = new CDirectXWrapper();
}

CLuaWrapper::~CLuaWrapper()
{
	Close();

	delete m_pDirectXWrapper;
}

bool CLuaWrapper::IsOpen()
{
	return (m_pLuaState != NULL);
}

bool CLuaWrapper::Close()
{
	if (IsOpen())
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;

		return true;
	}

	return false;
}

bool CLuaWrapper::Open(const char * fn)
{
	Close();

	m_pLuaState = luaL_newstate();
	luaL_openlibs(m_pLuaState);

	*static_cast<CLuaWrapper**>(lua_getextraspace(m_pLuaState)) = this;
	
	lua_register(m_pLuaState, "GetDisplayWidth", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetDisplayWidth>);
	lua_register(m_pLuaState, "GetDisplayHeight", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetDisplayHeight>);
	lua_register(m_pLuaState, "GetDisplayFrequency", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetDisplayFrequency>);
	lua_register(m_pLuaState, "SetResolution", &NLuaWrapper::dispatch<&CLuaWrapper::LuaSetResolution>);
	lua_register(m_pLuaState, "GetWidth", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetWidth>);
	lua_register(m_pLuaState, "GetHeight", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetHeight>);
	lua_register(m_pLuaState, "GetFPS", &NLuaWrapper::dispatch<&CLuaWrapper::LuaGetFPS>);
	lua_register(m_pLuaState, "LoadImage", &NLuaWrapper::dispatch<&CLuaWrapper::LuaLoadImage>);
	lua_register(m_pLuaState, "DrawSprite", &NLuaWrapper::dispatch<&CLuaWrapper::LuaDrawSprite>);
	lua_register(m_pLuaState, "LoadFont", &NLuaWrapper::dispatch<&CLuaWrapper::LuaLoadFont>);
	lua_register(m_pLuaState, "DrawText", &NLuaWrapper::dispatch<&CLuaWrapper::LuaDrawText>);
	lua_register(m_pLuaState, "LoadSound", &NLuaWrapper::dispatch<&CLuaWrapper::LuaLoadSound>);
	lua_register(m_pLuaState, "PlaySound", &NLuaWrapper::dispatch<&CLuaWrapper::LuaPlaySound>);
	
	if (luaL_dofile(m_pLuaState, fn) != LUA_OK)
	{
		Close();

		return false;
	}
	
	return true;
}

void CLuaWrapper::SetFPS(int iFPS)
{
	m_iFPS = std::max(iFPS, MINIMUM_FPS);
}

void CLuaWrapper::OnDestroy()
{
	if (!IsOpen()) return;

	lua_getglobal(m_pLuaState, "OnDestroy");

	lua_call(m_pLuaState, 0, 0);
}

void CLuaWrapper::OnUpdate(double dt)
{
	if (!IsOpen()) return;

	lua_getglobal(m_pLuaState, "OnUpdate");
	lua_pushnumber(m_pLuaState, dt);
	lua_call(m_pLuaState, 1, 0);
}

HBITMAP CLuaWrapper::OnRender(BYTE *pData, BITMAPINFO *pHeader)
{
	if (!IsOpen()) return NULL;

	// Clear render target
	m_pDirectXWrapper->Clear();

	// Execute draw commands from Lua
	lua_getglobal(m_pLuaState, "OnRender");
	lua_call(m_pLuaState, 0, 0);

	// Check if sprite batch is still active
	if (m_previousRenderCall.find("DrawSprite") != std::string::npos)
	{
		// End sprite batch
		m_pDirectXWrapper->EndSpriteBatch();
	}

	// Render to texture
	m_pDirectXWrapper->Flush();

	// Reset tracked render call
	m_previousRenderCall = "";

	//return NULL;
	return m_pDirectXWrapper->Capture(pData, pHeader);
}

int CLuaWrapper::LuaGetWidth(lua_State *L)
{
	lua_pushinteger(L, GetWidth());

	return 1;
}

int CLuaWrapper::LuaGetHeight(lua_State *L)
{
	lua_pushinteger(L, GetHeight());

	return 1;
}

int CLuaWrapper::LuaGetFPS(lua_State *L)
{
	lua_pushinteger(L, GetFPS());

	return 1;
}

int CLuaWrapper::LuaSetResolution(lua_State *L)
{
	SetResolution((int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2));
	SetFPS((int)luaL_checkinteger(L, 3));

	return 0;
}

int CLuaWrapper::LuaGetDisplayWidth(lua_State *L)
{
	HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	int width = GetDeviceCaps(hDC, HORZRES);
	DeleteDC(hDC);

	lua_pushinteger(L, width);

	return 1;
}

int CLuaWrapper::LuaGetDisplayHeight(lua_State *L)
{
	HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	int height = GetDeviceCaps(hDC, VERTRES);
	DeleteDC(hDC);
	
	lua_pushinteger(L, height);

	return 1;
}

int CLuaWrapper::LuaGetDisplayFrequency(lua_State *L)
{
	HDC hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	int frequency = GetDeviceCaps(hDC, VREFRESH);
	DeleteDC(hDC);

	lua_pushinteger(L, frequency);

	return 1;
}

int CLuaWrapper::LuaLoadImage(lua_State *L)
{
	const char *filePath = luaL_checkstring(L, 1);

	int index = m_pDirectXWrapper->LoadImage(to_wstring(filePath).c_str());

	lua_pushinteger(L, index);

	return 1;
}

int CLuaWrapper::LuaDrawSprite(lua_State *L)
{
	if (m_previousRenderCall.find(__FUNCTION__) == std::string::npos)
	{
		// Begin sprite batch
	    m_pDirectXWrapper->BeginSpriteBatch();
	}

	// Get arguments
	long long index = luaL_checkinteger(L, 1);
	double xPosition = luaL_checknumber(L, 2);
	double yPosition = luaL_checknumber(L, 3);
	
	// Draw sprite
    m_pDirectXWrapper->DrawSprite(int(index), float(xPosition), float(yPosition));

	//
	m_previousRenderCall = __FUNCTION__;

	return 0;
}

int CLuaWrapper::LuaLoadFont(lua_State * L)
{
	return 0;
}

int CLuaWrapper::LuaDrawText(lua_State * L)
{
	if (m_previousRenderCall.find("DrawSprite") != std::string::npos)
	{
		// End previous sprite batch
		m_pDirectXWrapper->EndSpriteBatch();
	}

	// Draw text

	m_previousRenderCall = __FUNCTION__;

	return 0;
}

int CLuaWrapper::LuaLoadSound(lua_State *L)
{
	return 0;
}

int CLuaWrapper::LuaPlaySound(lua_State *L)
{
	return 0;
}
