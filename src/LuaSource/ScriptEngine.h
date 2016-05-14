#pragma once


#include <lua.hpp>

#include "DirectXWrapper.h"


class CScriptEngine
{
public:
	CScriptEngine();
	~CScriptEngine();

	bool IsOpen();

	bool Close();
	bool Open(const char * fn);

	int GetWidth() { return m_pDirectXWrapper->GetWidth(); }
	void SetWidth(int iWidth) { m_pDirectXWrapper->SetWidth(iWidth); }
	int GetHeight() { return m_pDirectXWrapper->GetHeight(); }
	void SetHeight(int iHeight) { m_pDirectXWrapper->SetHeight(iHeight); }
	int GetFPS() { return m_iFPS; }
	void SetFPS(int iFPS);

	void OnDestroy();
	void OnUpdate(double dt);
	void OnRender();
protected:
	int m_iFPS;

	int SetResolution(lua_State *L);
	int LoadImage(lua_State *L);
	int DrawSprite(lua_State *L);
	int LoadFont(lua_State *L);
	int DrawText(lua_State *L);
	int LoadSound(lua_State *L);
	int PlaySound(lua_State *L);

	lua_State *m_pLua;

	CDirectXWrapper *m_pDirectXWrapper;
};
