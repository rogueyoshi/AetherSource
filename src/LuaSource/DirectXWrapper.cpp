#include <algorithm>
#define NOMINMAX

#include <stdlib.h>
#include <wincodec.h>
#include <DirectXColors.h>

#include "DirectXWrapper.h"
#include "StringHelper.h"
#include "SimpleMath.h"
#include "WICTextureLoader.h"
#include "ScreenGrab.h"

#define MINIMUM_WIDTH 1
#define MINIMUM_HEIGHT 1

using namespace DirectX::SimpleMath;

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

HHOOK CDirectXWrapper::m_hHook = NULL;

CDirectXWrapper::CDirectXWrapper() :
	m_iWidth(MINIMUM_WIDTH),
	m_iHeight(MINIMUM_HEIGHT),
	renderTargetFormat(DXGI_FORMAT_B8G8R8A8_UNORM), //DXGI_FORMAT_R8G8B8A8_UNORM
	depthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
{
	CreateDevice();
	CreateResources();

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());

	m_keyboard = std::make_unique<Keyboard>();
	m_gamePad = std::make_unique<GamePad>();

	FW1CreateFactory(FW1_VERSION, m_fw1FontFactory.ReleaseAndGetAddressOf());

	m_pHookThread = new std::thread(&Hook);
	m_pHookThread->detach();
}

CDirectXWrapper::~CDirectXWrapper()
{
	UnhookWindowsHookEx(m_hHook);
	delete m_pHookThread;
}

void CDirectXWrapper::SetResolution(int iWidth, int iHeight)
{
	iWidth = std::max(iWidth, MINIMUM_WIDTH);
	iHeight = std::max(iHeight, MINIMUM_HEIGHT);

	bool bChanged;
	if (iWidth != m_iWidth || iHeight != m_iHeight) bChanged = true;

	m_iWidth = iWidth;
	m_iHeight = iHeight;

	if (bChanged) CreateResources();
}

Image CDirectXWrapper::LoadImage(const wchar_t * filePath)
{
	Image texture;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), filePath, nullptr, &texture)
	);

	return texture;
}

void CDirectXWrapper::ReleaseImage(Image image)
{
	image->Release();
	//delete image;
}

void CDirectXWrapper::DrawSprite(Image image, float xPosition, float yPosition)
{
	m_spriteBatch->Draw(image, Vector2{ xPosition, yPosition });
}

Font CDirectXWrapper::LoadFont(LPCWSTR fontFamily)
{
	Font font;
	m_fw1FontFactory->CreateFontWrapper(m_d3dDevice.Get(), fontFamily, &font);

	return font;
}

void CDirectXWrapper::ReleaseFont(Font font)
{
	font->Release();
	//delete font;
}

void CDirectXWrapper::DrawText(const WCHAR *text, Font font, FLOAT size, FLOAT x, FLOAT y, UINT32 color)
{
	font->DrawString(
		m_d3dContext.Get(),
		text, // String
		size, // Font size
		x, // X position
		y, // Y position
		color, // Text color, 0xAaBbGgRr
		FW1_RESTORESTATE); // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
};

void CDirectXWrapper::Clear()
{
	// Clear the views.
	m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::Transparent);
	m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set render target.
	m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

	// Set the viewport.
	m_viewport = CD3D11_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));
	m_d3dContext->RSSetViewports(1, &m_viewport);
}

void CDirectXWrapper::BeginSpriteBatch()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonStates->NonPremultiplied());
}

void CDirectXWrapper::EndSpriteBatch()
{
	m_spriteBatch->End();
}

void CDirectXWrapper::Render()
{
	m_d3dContext->Flush();
}

// TODO: Figure out why the alpha channel is not reflected in the outgoing bitmap.
HBITMAP CDirectXWrapper::Capture()
{
	D3D11_TEXTURE2D_DESC textureDesc = m_renderTargetDesc;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	//textureDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texture;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, texture.ReleaseAndGetAddressOf())
	);
	m_d3dContext->CopyResource(texture.Get(), m_renderTarget.Get());

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	m_d3dContext->Map(texture.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ_WRITE, 0, &mappedSubresource);

	// Copy subresource data to buffer.
	static std::vector<uint8_t> buffer;
	buffer.resize(textureDesc.Width * textureDesc.Height * 4);
	uint8_t *source = reinterpret_cast<uint8_t *>(mappedSubresource.pData);
	uint8_t* destination = buffer.data();
	size_t size = std::min<size_t>(textureDesc.Width * 4, mappedSubresource.RowPitch);

	for (size_t h = 0; h < textureDesc.Height; h++)
	{
		memcpy_s(destination, textureDesc.Width * 4, source, size);
		source += mappedSubresource.RowPitch;
		destination += textureDesc.Width * 4;
	}

	destination -= textureDesc.Width * textureDesc.Height * 4;

	// Copy buffer to bitmap.
	HDC hDC = GetDC(NULL);
	HBITMAP	hBitmap = CreateCompatibleBitmap(hDC, textureDesc.Width, textureDesc.Height);
	SetBitmapBits(hBitmap, textureDesc.Width * textureDesc.Height * 4, destination);
	ReleaseDC(NULL, hDC);

	return hBitmap;
}

void CDirectXWrapper::Screenshot(LPCWSTR fileName)
{
	DX::ThrowIfFailed(
		SaveWICTextureToFile(m_d3dContext.Get(), m_renderTarget.Get(), GUID_ContainerFormatPng, fileName)
	);
}

void CDirectXWrapper::CreateDevice()
{
	DX::ThrowIfFailed(
		D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			NULL,
			D3D11_SDK_VERSION,
			m_d3dDevice.ReleaseAndGetAddressOf(),
			&m_featureLevel,
			m_d3dContext.ReleaseAndGetAddressOf())
	);

	m_commonStates = std::make_unique<CommonStates>(m_d3dDevice.Get());
}

void CDirectXWrapper::CreateResources()
{
	ID3D11RenderTargetView *nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_renderTargetView.Reset();
	m_depthStencilView.Reset();
	m_d3dContext->Flush();

	m_renderTargetDesc = CD3D11_TEXTURE2D_DESC(renderTargetFormat, m_iWidth, m_iHeight, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_renderTargetDesc.MiscFlags = 0;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(&m_renderTargetDesc, nullptr, m_renderTarget.GetAddressOf())
	);
	DX::ThrowIfFailed(
		m_d3dDevice->CreateRenderTargetView(m_renderTarget.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf())
	);

	depthStencilDesc = CD3D11_TEXTURE2D_DESC(depthStencilFormat, m_iWidth, m_iHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));
}

// Install hooks and create Windows messenging thread.
void CDirectXWrapper::Hook()
{
	HOOKPROC hookProc = [](int message, WPARAM wParam, LPARAM lParam) -> LRESULT {
		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(message, wParam, lParam);
			break;
		}

		return CallNextHookEx(m_hHook, message, wParam, lParam);
	};
	m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, hookProc, NULL, 0);

	// Message loop.
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0));
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
