#pragma once

#include "Pin.h"

class CFilter : public CSource
{
private:
	CFilter(IUnknown *pUnk, HRESULT *pHr);
	~CFilter();

	//CPin *m_pPin;
public:
	static CUnknown * WINAPI CFilter::CreateInstance(IUnknown *pUnk, HRESULT *pHr);

	//
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Stop();
	STDMETHODIMP GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *State);

	//STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	//ULONG STDMETHODCALLTYPE AddRef() { return CSource::AddRef(); };
	//ULONG STDMETHODCALLTYPE Release() { return CSource::Release(); };
};
