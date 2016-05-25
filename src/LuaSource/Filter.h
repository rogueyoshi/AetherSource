#pragma once

#include "Pin.h"

class CFilter : public CSource
{
public:
	CFilter(IUnknown *pUnk, HRESULT *pHr);
	~CFilter();
	
	//
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Stop();
	STDMETHODIMP GetState(DWORD dwMilliSecsTimeout, FILTER_STATE *State);

	//STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	//ULONG STDMETHODCALLTYPE AddRef() { return CSource::AddRef(); };
	//ULONG STDMETHODCALLTYPE Release() { return CSource::Release(); };
private:	
	//CPin *m_pPin;
};
