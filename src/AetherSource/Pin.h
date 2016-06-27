#pragma once

#include <streams.h>

#include "LuaWrapper.h"

class CPin : public CSourceStream, public IAMStreamConfig, public IKsPropertySet
{
protected:
	//int m_FramesWritten;				// To track where we are in the file
	//BOOL m_bZeroMemory;                // Do we need to clear the buffer?
	//CRefTime m_rtSampleTime;	       // The time stamp for each sample

	int m_iFrameNumber;

	REFERENCE_TIME m_rtFrameLength;

	DWORD m_iPreviousTime;
	//RECT m_rScreen;                    // Rect containing entire screen coordinates

	//int m_iHeight;                // The current image height
	//int m_iWidth;                 // And current image width
	//int m_iCurrentBitDepth;            // Screen bit depth
	//int m_iRepeatTime;                 // Time in msec between frames

	//CMediaType m_MediaType;
	//CCritSec m_cSharedState;           // Protects our internal state
	//CImageDisplay m_Display;           // Figures out our media type for us

	CLuaWrapper *m_pLuaWrapper;
public:
	CPin(HRESULT *pHr, CSource *pFilter);
	~CPin();

	void resetResources();

	HRESULT OnThreadCreate(void);
	HRESULT OnThreadDestroy(void);

	// Support multiple display formats
	HRESULT CheckMediaType(const CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition, CMediaType *pmt);

	// Set the agreed media type and set up the necessary parameters
	HRESULT SetMediaType(const CMediaType *pMediaType);

	// Override the version that offers exactly one media type
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
	HRESULT FillBuffer(IMediaSample *pSample);

	// Quality control
	// Not implemented because we aren't going in real time.
	// If the file-writing filter slows the graph down, we just do nothing, which means
	// wait until we're unblocked. No frames are ever dropped.
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q) { return E_FAIL; }

	//////////////////////////////////////////////////////////////////////////
	//  IUnknown
	//////////////////////////////////////////////////////////////////////////
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef() { return GetOwner()->AddRef(); }// gets called often...
	STDMETHODIMP_(ULONG) Release() { return GetOwner()->Release(); }

	//////////////////////////////////////////////////////////////////////////
	//  IAMStreamConfig
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE GetFormat(AM_MEDIA_TYPE **ppmt);
	HRESULT STDMETHODCALLTYPE SetFormat(AM_MEDIA_TYPE *pmt);
	HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(int *piCount, int *piSize);
	HRESULT STDMETHODCALLTYPE GetStreamCaps(int iIndex, AM_MEDIA_TYPE **ppmt, BYTE *pscc);

	//////////////////////////////////////////////////////////////////////////
	//  IKsPropertySet
	//////////////////////////////////////////////////////////////////////////
	HRESULT STDMETHODCALLTYPE Get(REFGUID guidPropSet, DWORD dwPropID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData, DWORD *pcbReturned);
	HRESULT STDMETHODCALLTYPE Set(REFGUID guidPropSet, DWORD dwID, void *pInstanceData, DWORD cbInstanceData, void *pPropData, DWORD cbPropData);
	HRESULT STDMETHODCALLTYPE QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport);
};
