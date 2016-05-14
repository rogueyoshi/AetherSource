#include <algorithm>
#define NOMINMAX

#include "DirectXWrapper.h"


CDirectXWrapper::CDirectXWrapper() :
	m_iWidth(1),
	m_iHeight(1)
{
}

CDirectXWrapper::~CDirectXWrapper()
{
}

void CDirectXWrapper::SetWidth(int iWidth)
{
	m_iWidth = std::max(iWidth, 1);
}

void CDirectXWrapper::SetHeight(int iHeight)
{
	m_iHeight = std::max(iHeight, 1);
}
