#pragma once


#include <wrl/client.h>

//#include <d3d11_1.h>
//#include <DirectXMath.h>

class CDirectXWrapper
{
public:
	CDirectXWrapper();
	~CDirectXWrapper();

	int GetWidth() { return m_iWidth; }
	void SetWidth(int iWidth);
	int GetHeight() { return m_iHeight; }
	void SetHeight(int iHeight);
protected:
	int m_iWidth;
	int m_iHeight;
};
