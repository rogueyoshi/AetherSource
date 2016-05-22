#pragma once

#include "Pin.h"

class CFilter : public CSource
{
public:
	static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *pHr);
	
	//
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Stop();
	STDMETHODIMP GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *State);

	//STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	//ULONG STDMETHODCALLTYPE AddRef() { return CSource::AddRef(); };
	//ULONG STDMETHODCALLTYPE Release() { return CSource::Release(); };
private:
	// Constructor is private because you have to use CreateInstance
	CFilter(IUnknown *pUnk, HRESULT *pHr);
	~CFilter();
	
	//CPin *m_pPin;
};
