#include "Filter.h"
#include "Guids.h"

const AMOVIESETUP_MEDIATYPE g_MediaTypeSetup =
{
	&MEDIATYPE_Video,  // Major type
					   //&MEDIASUBTYPE_ARGB32 // Minor type
					   &MEDIASUBTYPE_NULL // Minor type
};

const AMOVIESETUP_PIN g_PinSetup =
{
	NULL, // Obsolete, not used.
	FALSE, // Is this pin rendered?
	TRUE, // Is it an output pin?
	FALSE, // Can the filter create zero instances?
	FALSE, // Does the filter create multiple instances?
	&CLSID_NULL, // Obsolete.
	NULL, // Obsolete.
	1, // Number of media types.
	&g_MediaTypeSetup // Pointer to media types.
};

const AMOVIESETUP_FILTER g_FilterSetup =
{
	&CLSID_Filter, // Filter CLSID
	NAME(PROJECT_NAME), // String name
	MERIT_DO_NOT_USE, // Filter merit
	1, // Number of pins
	&g_PinSetup // Pin details
};

// DirectShow queries for this global
CFactoryTemplate g_Templates[] =
{
	{
		NAME(PROJECT_NAME), // Name
		&CLSID_Filter, // CLSID
		[](IUnknown *pUnk, HRESULT *pHr) -> CUnknown * { // Method to create an instance of LuaSource
		CFilter *pFilter = new CFilter(pUnk, pHr);

		if (pHr)
		{
			if (pFilter == NULL)
				*pHr = E_OUTOFMEMORY;
			else
				*pHr = S_OK;
		}

	return pFilter;
},
NULL, // Initialization function
&g_FilterSetup // Set-up information (for filters)
	}
};

// DirectShow queries for this global
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

REGFILTER2 g_rf2 =
{
	1, // Version 1 (no pin mediums or pin category).
	MERIT_DO_NOT_USE, // Merit.
	1, // Number of pins.
	&g_PinSetup // Pointer to pin information.
};

STDAPI DllUnregisterServer()
{
	HRESULT hr = AMovieDllRegisterServer2(FALSE);
	if (FAILED(hr))	return hr;

	IFilterMapper2 *pFM2 = NULL;
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&pFM2);
	if (FAILED(hr))	return hr;

	hr = pFM2->UnregisterFilter(
		&CLSID_VideoInputDeviceCategory,
		NAME(PROJECT_NAME),
		CLSID_Filter
	);

	pFM2->Release();

	return hr;
}

STDAPI DllRegisterServer()
{
	HRESULT hr = AMovieDllRegisterServer2(TRUE);
	if (FAILED(hr)) return hr;

	IFilterMapper2 *pFM2 = NULL;
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&pFM2);
	if (FAILED(hr)) return hr;

	hr = pFM2->RegisterFilter(
		CLSID_Filter, // Filter CLSID. 
		NAME(PROJECT_NAME), // Filter name.
		NULL, // Device moniker. 
		&CLSID_VideoInputDeviceCategory, // Video compressor category.
		NULL, // Instance data.
		&g_rf2 // Pointer to filter information.
	);

	pFM2->Release();

	return hr;
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule,
	DWORD  dwReason,
	LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
