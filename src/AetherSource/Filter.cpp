#include "Filter.h"
#include "Guids.h"
#include "GuiCon.h"

CUnknown * WINAPI CFilter::CreateInstance(IUnknown *pUnk, HRESULT *pHr)
{
	auto *pFilter = new CFilter(pUnk, pHr);

	if (pHr)
	{
		if (pFilter == nullptr)
			*pHr = E_OUTOFMEMORY;
		else
			*pHr = S_OK;
	}

	return pFilter;
}

STDMETHODIMP CFilter::Run(REFERENCE_TIME tStart)
{
	CAutoLock autoLock(m_pLock);

	return CSource::Run(tStart);
}

STDMETHODIMP CFilter::Stop()
{
	CAutoLock autoLock(m_pLock);

	//Reset pin resources
	//m_pPin->m_iFrameNumber = 0;

	for (int i = 0; i < m_iPins; i++)
	{
		auto pPin = (CPin *)&m_paStreams[i];
		pPin->resetResources();
	}

	return CSource::Stop();
}

HRESULT CFilter::GetState(DWORD dw, FILTER_STATE *pState)
{
	CheckPointer(pState, E_POINTER);

	*pState = m_State;

	if (m_State == State_Paused)
		return VFW_S_CANT_CUE;
	else
		return S_OK;
}

/*HRESULT CFilter::QueryInterface(REFIID riid, void **ppv)
{
	//Forward request for IAMStreamConfig & IKsPropertySet to the pin
	if (riid == _uuidof(IAMStreamConfig) || riid == _uuidof(IKsPropertySet))
		return m_pPin->QueryInterface(riid, ppv);
	else
		return CSource::QueryInterface(riid, ppv);
}*/

CFilter::CFilter(IUnknown *pUnk, HRESULT *pHr)
	: CSource(NAME("Filter"), pUnk, CLSID_Filter)
{
	redirectIOToConsole();
	//EnableMenuItem(GetConsoleWindow(), SC_CLOSE, MF_GRAYED);
	RemoveMenu(GetSystemMenu(GetConsoleWindow(), FALSE), SC_CLOSE, MF_BYCOMMAND);
	SetConsoleTitle(TEXT(PROJECT_NAME));

	// The pin magically adds itself to our pin array.
	//m_pPin = new CPin(pHr, this);
	new CPin(pHr, this);
	if (pHr)
	{
		/*if (m_pPin == nullptr)
		*pHr = E_OUTOFMEMORY;
		else
		*pHr = S_OK;*/

		*pHr = S_OK;

		for (int i = 0; i < m_iPins; i++)
		{
			if (m_paStreams[i] == nullptr) *pHr = E_OUTOFMEMORY;
		}
	}
}

CFilter::~CFilter()
{
	//delete m_pPin;

	for (int i = 0; i < m_iPins; i++)
	{
		delete m_paStreams[i];
	}

	m_iPins = 0;

	// Deallocate console
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeConsole();
}
