#include <algorithm>
#define NOMINMAX

#include "ScriptEngine.h"


// Wrap the lua C-style function into a namespace for disambiguation and future-proofing.
namespace ScriptEngine
{
	typedef int (CScriptEngine::* signature)(lua_State * L);

	// This template wraps a member function into a C-style "free" function compatible with lua.
	template <signature function>

	int dispatch(lua_State * L)
	{
		CScriptEngine * ptr = *static_cast<CScriptEngine**>(lua_getextraspace(L));

		return ((*ptr).*function)(L);
	}
}


CScriptEngine::CScriptEngine() :
	m_iFPS(1),
	m_pLua(NULL)
{
	m_pDirectXWrapper = new CDirectXWrapper();
}

CScriptEngine::~CScriptEngine()
{
	Close();

	delete m_pDirectXWrapper;
}

bool CScriptEngine::IsOpen()
{
	return (m_pLua != NULL);
}

bool CScriptEngine::Close()
{
	if (IsOpen())
	{
		lua_close(m_pLua);
		m_pLua = NULL;

		return true;
	}

	return false;
}

bool CScriptEngine::Open(const char * fn)
{
	Close();

	m_pLua = luaL_newstate();
	luaL_openlibs(m_pLua);

	*static_cast<CScriptEngine**>(lua_getextraspace(m_pLua)) = this;
	
	lua_register(m_pLua, "SetResolution", &ScriptEngine::dispatch<&CScriptEngine::SetResolution>);
	lua_register(m_pLua, "LoadImage", &ScriptEngine::dispatch<&CScriptEngine::LoadImage>);
	lua_register(m_pLua, "DrawSprite", &ScriptEngine::dispatch<&CScriptEngine::DrawSprite>);
	lua_register(m_pLua, "LoadFont", &ScriptEngine::dispatch<&CScriptEngine::LoadFont>);
	lua_register(m_pLua, "DrawText", &ScriptEngine::dispatch<&CScriptEngine::DrawText>);
	lua_register(m_pLua, "LoadSound", &ScriptEngine::dispatch<&CScriptEngine::LoadSound>);
	lua_register(m_pLua, "PlaySound", &ScriptEngine::dispatch<&CScriptEngine::PlaySound>);

	if (luaL_dofile(m_pLua, fn) != LUA_OK)
	{
		Close();

		return false;
	}
	
	return true;
}

void CScriptEngine::SetFPS(int iFPS)
{
	m_iFPS = std::max(iFPS, 1);
}

void CScriptEngine::OnDestroy()
{
	if (!IsOpen()) return;

	lua_getglobal(m_pLua, "OnDestroy");
	lua_call(m_pLua, 0, 0);
}

void CScriptEngine::OnUpdate(double dt)
{
	if (!IsOpen()) return;

	lua_getglobal(m_pLua, "OnUpdate");
	lua_pushnumber(m_pLua, dt);
	lua_call(m_pLua, 1, 0);
}

void CScriptEngine::OnRender()
{
	if (!IsOpen()) return;

	lua_getglobal(m_pLua, "OnRender");
	lua_call(m_pLua, 0, 0);
}

int CScriptEngine::SetResolution(lua_State * L)
{
	SetWidth((int)luaL_checkinteger(L, 1));
	SetHeight((int)luaL_checkinteger(L, 2));
	SetFPS((int)luaL_checkinteger(L, 3));

	return 0;
}

int CScriptEngine::LoadImage(lua_State * L)
{
	return 0;
}

int CScriptEngine::DrawSprite(lua_State * L)
{
	return 0;
}

int CScriptEngine::LoadFont(lua_State * L)
{
	return 0;
}

int CScriptEngine::DrawText(lua_State * L)
{
	return 0;
}

int CScriptEngine::LoadSound(lua_State * L)
{
	return 0;
}

int CScriptEngine::PlaySound(lua_State * L)
{
	return 0;
}
