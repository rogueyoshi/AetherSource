#include <algorithm>
#define NOMINMAX

#include <wincodec.h>
#include <DirectXColors.h>

#include "DirectXWrapper.h"
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

CDirectXWrapper::CDirectXWrapper() :
	m_iWidth(1),
	m_iHeight(1),
	textureFormat(DXGI_FORMAT_B8G8R8A8_UNORM), //DXGI_FORMAT_R8G8B8A8_UNORM
	depthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT)
{
	CreateDevice();
	CreateResources();

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
}

CDirectXWrapper::~CDirectXWrapper()
{

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

int CDirectXWrapper::LoadImage(const wchar_t * filePath)
{
	ComPtr<ID3D11ShaderResourceView> texture;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), filePath, nullptr, texture.ReleaseAndGetAddressOf())
	);

	m_sprites.push_back(texture);

	return m_sprites.size() - 1;
}

void CDirectXWrapper::DeleteImage(int index)
{
	m_sprites.erase(m_sprites.begin() + index);
}

void CDirectXWrapper::BeginSpriteBatch()
{
	m_spriteBatch->Begin(SpriteSortMode_Deferred, m_commonStates->NonPremultiplied());
}

void CDirectXWrapper::EndSpriteBatch()
{
	m_spriteBatch->End();
}

void CDirectXWrapper::DrawSprite(int index, float xPosition, float yPosition)
{
	m_spriteBatch->Draw(m_sprites.at(index).Get(), Vector2{ xPosition, yPosition });
}

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

void CDirectXWrapper::Flush()
{
	m_d3dContext->Flush();
}

void CDirectXWrapper::Screenshot()
{
	DX::ThrowIfFailed(
		SaveWICTextureToFile(m_d3dContext.Get(), m_texture.Get(), GUID_ContainerFormatPng, L"Screenshot.png")
	);
}

// TODO: Isolate and fix the leak in this function.
// TODO: Figure out why the alpha channel is not reflected in the outgoing pData
HBITMAP CDirectXWrapper::Capture()
{
	D3D11_TEXTURE2D_DESC textureDesc = m_textureDesc;
	textureDesc.Usage = D3D11_USAGE_STAGING;
	textureDesc.BindFlags = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	//textureDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> texture;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(&textureDesc, nullptr, texture.ReleaseAndGetAddressOf())
	);
	m_d3dContext->CopyResource(texture.Get(), m_texture.Get());
	
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	m_d3dContext->Map(texture.Get(), D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ_WRITE, 0, &mappedSubresource);
	
	// Copy subresource data to buffer.
	static std::vector<uint8_t> image_buffer;
	image_buffer.resize(textureDesc.Width * textureDesc.Height * 4);
	uint8_t *source = reinterpret_cast<uint8_t*>(mappedSubresource.pData);
	uint8_t* destination = image_buffer.data();
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

	m_textureDesc = CD3D11_TEXTURE2D_DESC(textureFormat, m_iWidth, m_iHeight, 1, 1, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_textureDesc.MiscFlags = 0;
	DX::ThrowIfFailed(
		m_d3dDevice->CreateTexture2D(&m_textureDesc, nullptr, m_texture.GetAddressOf())
	);
	DX::ThrowIfFailed(
		m_d3dDevice->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf())
	);

	depthStencilDesc = CD3D11_TEXTURE2D_DESC(depthStencilFormat, m_iWidth, m_iHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);
	DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

	depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D);
	DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));
}
