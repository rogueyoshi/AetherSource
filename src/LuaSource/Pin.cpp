#include "Pin.h"
#include "StringHelper.h"
#include "OpenFileDialog.h"

#define FPS(x) UNITS / x

CPin::CPin(HRESULT *pHr, CSource *pFilter) : CSourceStream(NAME("Pin"), pHr, pFilter, NAME("Output")),
	//m_FramesWritten(0),
	//m_bZeroMemory(0),
	m_iFrameNumber(0)
{
	// The main point of this sample is to demonstrate how to take a DIB
	// in host memory and insert it into a video stream. 

	// To keep this sample as simple as possible, we just read the image
	// from a file and copy it into every frame that we send downstream.
	//
	// In the filter graph, we connect this filter to the AVI Mux, which creates 
	// the AVI file with the video frames we pass to it. In this case, 
	// the end result is a screen capture video (GDI images only, with no
	// support for overlay surfaces).

	// Get the dimensions of the incoming bitmaps

	m_pLuaWrapper = new CLuaWrapper();

	static OpenFileDialog *pFileDialog = new OpenFileDialog();
	pFileDialog->Title = TEXT("Open Lua script");
	//pFileDialog->DefaultExtension = TEXT(".lua");
	pFileDialog->Filter = TEXT(".lua");

	if (!_tcslen(pFileDialog->FileName)) pFileDialog->ShowDialog();
	
	if (_tcslen(pFileDialog->FileName))
	{
		SetConsoleTitle(pFileDialog->FileName);
		m_pLuaWrapper->Open(to_string(pFileDialog->FileName).c_str());
	}

	// Get the dimensions
	//m_iWidth = m_pScriptEngine->GetWidth();
	//m_iHeight = m_pScriptEngine->GetHeight();
	m_rtFrameLength = FPS(m_pLuaWrapper->GetFPS());

	/*HDC hDc;
	hDc = CreateDC(NAME("DISPLAY"), NULL, NULL, NULL);
	m_iCurrentBitDepth = GetDeviceCaps(hDc, BITSPIXEL);
	DeleteDC(hDc);*/

	// Save dimensions of the main window for later use in FillBuffer()
	//m_rScreen.left = 0;
	//m_rScreen.top = 0;
	//m_rScreen.right = m_pLuaWrapper->GetWidth();
	//m_rScreen.bottom = m_pLuaWrapper->GetHeight();
}

CPin::~CPin()
{
	m_pLuaWrapper->OnDestroy();
	delete m_pLuaWrapper;

	//DbgLog((LOG_TRACE, 3, TEXT("Frames written %d"), m_iFrameNumber));
}

void CPin::resetResources()
{
	m_iFrameNumber = 0;
}

/*HRESULT CPushPin::OnThreadCreate()
{
return S_OK;
}*/

/*HRESULT CPushPin::OnThreadDestroy()
{
return S_OK;
}*/

//
// CheckMediaType
//
// We will accept 8, 16, 24 or 32 bit video formats, in any
// image size that gives room to bounce.
// Returns E_INVALIDARG if the mediatype is not acceptable
//
HRESULT CPin::CheckMediaType(const CMediaType *pMediaType)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	CheckPointer(pMediaType, E_POINTER);

	if ((*(pMediaType->Type()) != MEDIATYPE_Video) || !(pMediaType->IsFixedSize())) // we only output video in fixed size samples
	{
		return E_INVALIDARG;
	}

	// Check for the subtypes we support
	const GUID *SubType = pMediaType->Subtype();

	if (SubType == NULL) return E_INVALIDARG;

	if (
		(*SubType != MEDIASUBTYPE_ARGB32) &&
		(*SubType != MEDIASUBTYPE_RGB32) &&
		(*SubType != MEDIASUBTYPE_RGB24) &&
		(*SubType != MEDIASUBTYPE_RGB565) &&
		(*SubType != MEDIASUBTYPE_RGB555) &&
		(*SubType != MEDIASUBTYPE_RGB8)
	)
	{
		return E_INVALIDARG;
	}

	// Get the format area of the media type
	VIDEOINFO *pvi = (VIDEOINFO *)pMediaType->Format();
	if (pvi == NULL) return E_INVALIDARG;

	// Don't accept formats with negative height, which would cause the 
	// image to be displayed upside down.
	if (pvi->bmiHeader.biHeight < 0) return E_INVALIDARG;

	// Check if the image width & height have changed
	if (pvi->bmiHeader.biWidth != m_pLuaWrapper->GetWidth() || abs(pvi->bmiHeader.biHeight) != m_pLuaWrapper->GetHeight())
	{
		// If the image width/height is changed, fail CheckMediaType() to force
		// the renderer to resize the image.
		return E_INVALIDARG;
	}

	return S_OK;  // This format is acceptable.

} // CheckMediaType

  //
  // GetMediaType
  //
  // Prefer 5 formats - 8, 16 (*2), 24 or 32 bits per pixel
  //
  // Prefered types should be ordered by quality, with zero as highest quality.
  // Therefore, iPosition =
  //      0    Return a 32bit mediatype
  //      1    Return a 24bit mediatype
  //      2    Return 16bit RGB565
  //      3    Return a 16bit mediatype (rgb555)
  //      4    Return 8 bit palettised format
  //      >4   Invalid
  //
HRESULT CPin::GetMediaType(int iPosition, CMediaType *pMt)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	CheckPointer(pMt, E_POINTER);

	if (iPosition < 0) return E_INVALIDARG;

	// Have we run off the end of types?
	if (iPosition > 5) return VFW_S_NO_MORE_ITEMS;

	VIDEOINFO *pvi = (VIDEOINFO *)pMt->AllocFormatBuffer(sizeof(VIDEOINFO));
	if (!pvi) return(E_OUTOFMEMORY);

	// Initialize the VideoInfo structure before configuring its members
	ZeroMemory(pvi, sizeof(VIDEOINFO));

	// Return our highest quality 32bit format

	// Since we use RGB888 (the default for 32 bit), there is
	// no reason to use BI_BITFIELDS to specify the RGB
	// masks. Also, not everything supports BI_BITFIELDS
	pvi->bmiHeader.biCompression = BI_RGB;
	pvi->bmiHeader.biBitCount = 32;

	// Adjust the parameters common to all formats
	pvi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pvi->bmiHeader.biWidth = m_pLuaWrapper->GetWidth();
	pvi->bmiHeader.biHeight = m_pLuaWrapper->GetHeight();
	pvi->bmiHeader.biPlanes = 1;
	pvi->bmiHeader.biSizeImage = GetBitmapSize(&pvi->bmiHeader);
	pMt->SetSampleSize(pvi->bmiHeader.biSizeImage);

	pvi->bmiHeader.biClrImportant = 0;
	pvi->AvgTimePerFrame = m_rtFrameLength;

	SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered.
	SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

	pMt->SetType(&MEDIATYPE_Video);
	pMt->SetFormatType(&FORMAT_VideoInfo);
	pMt->SetTemporalCompression(FALSE);

	// Work out the GUID for the subtype from the header info.
	// TODO: Figure out how to get it to accept the MEDIASUBTYPE_ARGB32 subtype
	if (*pMt->Subtype() == GUID_NULL)
	{
		const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
		pMt->SetSubtype(&SubTypeGUID);
	}

	return NOERROR;

} // GetMediaType

  //
  // SetMediaType
  //
  // Called when a media type is agreed between filters
  //
HRESULT CPin::SetMediaType(const CMediaType *pMediaType)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	// Pass the call up to my base class
	HRESULT hr = CSourceStream::SetMediaType(pMediaType);

	if (SUCCEEDED(hr))
	{
		VIDEOINFO * pvi = (VIDEOINFO *)m_mt.Format();
		if (pvi == NULL) return E_UNEXPECTED;

		if (pvi->bmiHeader.biBitCount == 32)
		{
			hr = S_OK;
		}
		else
		{
			hr = E_INVALIDARG;
			ASSERT(FALSE);
		}

		if (pvi->AvgTimePerFrame) m_rtFrameLength = pvi->AvgTimePerFrame;
	}

	return hr;
} // SetMediaType

  //
  // DecideBufferSize
  //
  // This will always be called after the format has been sucessfully
  // negotiated. So we have a look at m_mt to see what size image we agreed.
  // Then we can ask for buffers of the correct size to contain them.
  //
HRESULT CPin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	CheckPointer(pAlloc, E_POINTER);
	CheckPointer(pProperties, E_POINTER);

	VIDEOINFO *pvi = (VIDEOINFO *)m_mt.Format();
	pProperties->cBuffers = 1;
	pProperties->cbBuffer = pvi->bmiHeader.biSizeImage;
	ASSERT(pProperties->cbBuffer);

	// Ask the allocator to reserve us some sample memory. NOTE: the function
	// can succeed (return NOERROR) but still not have allocated the
	// memory that we requested, so we must check we got whatever we wanted.
	ALLOCATOR_PROPERTIES Actual;
	HRESULT hr = pAlloc->SetProperties(pProperties, &Actual);

	if (FAILED(hr))	return hr;

	// Is this allocator unsuitable?
	if (Actual.cbBuffer < pProperties->cbBuffer) return E_FAIL;

	// Make sure that we have only 1 buffer (we erase the ball in the
	// old buffer to save having to zero a 200k+ buffer every time
	// we draw a frame)
	ASSERT(Actual.cBuffers == 1);

	return NOERROR;
} // DecideBufferSize

  // This is where we insert the DIB bits into the video stream.
  // FillBuffer is called once for every sample in the stream.
HRESULT CPin::FillBuffer(IMediaSample *pSample)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	CheckPointer(pSample, E_POINTER);

	/*IUnknown *pUnknown = GetOwner();
	CFilter *pOwner = (CFilter *)&pUnknown;
	FILTER_STATE state;
	pOwner->GetState(INFINITE, &state);

	while (state != State_Running)
	{
	// TODO: accomodate for pausing better, we're single run only currently [does VLC do pausing even?]
	Sleep(1);

	pOwner->GetState(INFINITE, &state);
	}*/

	// Access the sample's data buffer
	BYTE *pData;
	pSample->GetPointer(&pData);

	long cbData;
	cbData = pSample->GetSize();

	// Check that we're still using video
	ASSERT(m_mt.formattype == FORMAT_VideoInfo);

	VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)m_mt.pbFormat;

	// Update and Render from Lua
	DWORD currentTime = GetCurrentTime();
	static DWORD previousTime = currentTime;
	double deltaTime = (currentTime - previousTime) * 0.001;
	m_pLuaWrapper->OnUpdate(deltaTime);
	m_pLuaWrapper->OnRender(deltaTime);

	// Capture output from Lua's DirectX instance
	HBITMAP hBitmap = m_pLuaWrapper->Capture();

	// Copy the DIB bits over into our filter's output buffer.
	// Since sample size may be larger than the image size, bound the copy size.
	//int nSize = min(pVih->bmiHeader.biSizeImage, (DWORD)cbData);
	HDC hDC = GetDC(NULL);
	GetDIBits(hDC, hBitmap, 0, m_pLuaWrapper->GetHeight(), pData, (BITMAPINFO *)&(pVih->bmiHeader), DIB_RGB_COLORS);
	ReleaseDC(NULL, hDC);

	if (hBitmap) DeleteObject(hBitmap);

	// Set the timestamps that will govern playback frame rate.
	// If this file is getting written out as an AVI,
	// then you'll also need to configure the AVI Mux filter to 
	// set the Average Time Per Frame for the AVI Header.
	// The current time is the sample's start.
	REFERENCE_TIME rtStart = m_iFrameNumber * m_rtFrameLength;
	REFERENCE_TIME rtStop = rtStart + m_rtFrameLength;
	pSample->SetTime(&rtStart, &rtStop);

	// Increment frame number
	m_iFrameNumber++;

	// Set TRUE on every sample for uncompressed frames
	pSample->SetSyncPoint(TRUE);

	// Set time for delta
	previousTime = GetCurrentTime();

	return S_OK;
}

HRESULT CPin::QueryInterface(REFIID riid, void **ppv)
{
	// Standard OLE stuff, needed for capture source
	if (riid == _uuidof(IAMStreamConfig))
		*ppv = (IAMStreamConfig*)this;
	else if (riid == _uuidof(IKsPropertySet))
		*ppv = (IKsPropertySet*)this;
	else
		return CSourceStream::QueryInterface(riid, ppv);

	AddRef(); // avoid interlocked decrement error... // I think

	return S_OK;
}

// Get's the current format...I guess...
// or get default if they haven't called SetFormat yet...
// LODO the default, which probably we don't do yet...unless they've already called GetStreamCaps then it'll be the last index they used LOL.
HRESULT STDMETHODCALLTYPE CPin::GetFormat(AM_MEDIA_TYPE **ppMt)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	*ppMt = CreateMediaType(&m_mt); // windows internal method, also does copy

	return S_OK;
}

// sets fps, size, (etc.) maybe, or maybe just saves it away for later use...
HRESULT STDMETHODCALLTYPE CPin::SetFormat(AM_MEDIA_TYPE *pMt)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	// I *think* it can go back and forth, then.  You can call GetStreamCaps to enumerate, then call
	// SetFormat, then later calls to GetMediaType/GetStreamCaps/EnumMediatypes will all "have" to just give this one
	// though theoretically they could also call EnumMediaTypes, then Set MediaType, and not call SetFormat
	// does flash call both? what order for flash/ffmpeg/vlc calling both?
	// LODO update msdn

	// "they" [can] call this...see msdn for SetFormat

	// NULL means reset to default type...
	if (pMt != NULL)
	{
		if (pMt->formattype != FORMAT_VideoInfo) return E_FAIL;

		// LODO I should do more here...http://msdn.microsoft.com/en-us/library/dd319788.aspx I guess [meh]
		// LODO should fail if we're already streaming... [?]

		if (CheckMediaType((CMediaType *)pMt) != S_OK) return E_FAIL; // just in case :P [FME...]

		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pMt->pbFormat;

		// for FMLE's benefit, only accept a setFormat of our "final" width [force setting via registry I guess, otherwise it only shows 80x60 whoa!]	    
		// flash media live encoder uses setFormat to determine widths [?] and then only displays the smallest? huh?
		if (pvi->bmiHeader.biWidth != m_pLuaWrapper->GetWidth() || pvi->bmiHeader.biHeight != m_pLuaWrapper->GetHeight())
			return E_INVALIDARG;

		// ignore other things like cropping requests for now...

		// now save it away...for being able to re-offer it later. We could use Set MediaType but we're just being lazy and re-using m_mt for many things I guess
		m_mt = *pMt;
	}

	IPin* pPin;
	ConnectedTo(&pPin);

	if (pPin)
	{
		HRESULT hr = m_pFilter->GetFilterGraph()->Reconnect(this);
		if (hr != S_OK) return hr; // LODO check first, and then just re-use the old one?
								   // else return early...not really sure how to handle this...since we already set m_mt...but it's a pretty rare case I think...
								   // plus ours is a weird case...
	}
	else
	{
		// graph hasn't been built yet...
		// so we're ok with "whatever" format they pass us, we're just in the setup phase...
	}

	// success of some type
	if (pMt == NULL)
	{
		//m_bFormatAlreadySet = false;
	}
	else
	{
		//m_bFormatAlreadySet = true;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPin::GetNumberOfCapabilities(int *piCount, int *piSize)
{
	*piCount = 1;
	*piSize = sizeof(VIDEO_STREAM_CONFIG_CAPS); // VIDEO_STREAM_CONFIG_CAPS is an MS struct

	return S_OK;
}

// returns the "range" of fps, etc. for this index
HRESULT STDMETHODCALLTYPE CPin::GetStreamCaps(int iIndex, AM_MEDIA_TYPE **pMt, BYTE *pSCC)
{
	CAutoLock autoLock(m_pFilter->pStateLock());

	HRESULT hr = GetMediaType(iIndex, &m_mt); // ensure setup/re-use m_mt... some are indeed shared, apparently.
	if (FAILED(hr))	return hr;

	*pMt = CreateMediaType(&m_mt); // a windows lib method, also does a copy for us
	if (*pMt == NULL) return E_OUTOFMEMORY;

	VIDEO_STREAM_CONFIG_CAPS * pVSCC = (VIDEO_STREAM_CONFIG_CAPS*)(pSCC);

	/*
	most of these are listed as deprecated by msdn... yet some still used, apparently. odd.
	*/

	pVSCC->VideoStandard = AnalogVideo_None;

	pVSCC->InputSize.cx = pVSCC->MinCroppingSize.cx = pVSCC->MaxCroppingSize.cx = pVSCC->MaxOutputSize.cx = m_pLuaWrapper->GetWidth();
	pVSCC->InputSize.cy = pVSCC->MinCroppingSize.cy = pVSCC->MaxCroppingSize.cy = pVSCC->MaxOutputSize.cy = m_pLuaWrapper->GetHeight();

	pVSCC->CropGranularityX = pVSCC->CropGranularityY = 1;
	pVSCC->CropAlignX = pVSCC->CropAlignY = 1;

	pVSCC->MinOutputSize.cx = pVSCC->MinOutputSize.cy = 1;
	pVSCC->OutputGranularityX = pVSCC->OutputGranularityY = 1;

	pVSCC->StretchTapsX = pVSCC->StretchTapsY = 1;
	pVSCC->ShrinkTapsX = pVSCC->ShrinkTapsY = 1;

	pVSCC->MinFrameInterval = m_rtFrameLength;
	pVSCC->MaxFrameInterval = FPS(6);

	pVSCC->MinBitsPerSecond = pVSCC->MinOutputSize.cx * pVSCC->MinOutputSize.cy * 8 * FPS((LONG)pVSCC->MinFrameInterval) + sizeof(VIDEOINFO); // if in 8 bit mode 1x1. I guess.
	pVSCC->MaxBitsPerSecond = pVSCC->MaxOutputSize.cx * pVSCC->MaxOutputSize.cy * 32 * FPS((LONG)pVSCC->MinFrameInterval) + sizeof(VIDEOINFO); // + 44 header size? + the palette?

	return hr;
}

// Get: Return the pin category (our only property). 
HRESULT CPin::Get(
	REFGUID guidPropSet, // Which property set.
	DWORD dwPropID, // Which property in that set.
	void *pInstanceData, // Instance data (ignore).
	DWORD cbInstanceData, // Size of the instance data (ignore).
	void *pPropData, // Buffer to receive the property data.
	DWORD cbPropData, // Size of the buffer.
	DWORD *pcbReturned // Return the size of the property.
)
{
	if (guidPropSet != AMPROPSETID_Pin)             return E_PROP_SET_UNSUPPORTED;
	if (dwPropID != AMPROPERTY_PIN_CATEGORY)        return E_PROP_ID_UNSUPPORTED;
	if (pPropData == NULL && pcbReturned == NULL)   return E_POINTER;

	if (pcbReturned) *pcbReturned = sizeof(GUID);
	if (pPropData == NULL)          return S_OK; // Caller just wants to know the size. 
	if (cbPropData < sizeof(GUID))  return E_UNEXPECTED;// The buffer is too small.

	*(GUID *)pPropData = PIN_CATEGORY_CAPTURE; // PIN_CATEGORY_PREVIEW ?

	return S_OK;
}

HRESULT CPin::Set(
	REFGUID guidPropSet,
	DWORD dwID,
	void *pInstanceData,
	DWORD cbInstanceData,
	void *pPropData,
	DWORD cbPropData
)
{
	// Set: we don't have any specific properties to set...that we advertise yet anyway, and who would use them anyway?
	return E_NOTIMPL;
}

HRESULT CPin::QuerySupported(REFGUID guidPropSet, DWORD dwPropID, DWORD *pTypeSupport)
{
	if (guidPropSet != AMPROPSETID_Pin) return E_PROP_SET_UNSUPPORTED;
	if (dwPropID != AMPROPERTY_PIN_CATEGORY) return E_PROP_ID_UNSUPPORTED;

	// We support getting this property, but not setting it.
	if (pTypeSupport) *pTypeSupport = KSPROPERTY_SUPPORT_GET;

	return S_OK;
}
