#pragma once

#include <vector>
#include <thread>
#include <wrl/client.h>
#include <d3d11_1.h>

#include "CommonStates.h"
#include "SpriteBatch.h"
#include "Keyboard.h"
#include "GamePad.h"
#include "FW1FontWrapper.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace DX
{
	typedef ID3D11ShaderResourceView *Image;
	typedef IFW1FontWrapper *Font;

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DirectX API errors
			throw std::exception();
		}
	}
}

// "I'm not a wrapper."
class CDirectXWrapper
{
public:
	CDirectXWrapper();
	~CDirectXWrapper();

	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	void SetResolution(int iWidth, int iHeight);

	DX::Image LoadImage(const wchar_t *filePath);
	void ReleaseImage(DX::Image image);
	void DrawSprite(DX::Image image, float xPosition, float yPosition);

	DX::Font LoadFont(LPCWSTR fontFamily);
	void ReleaseFont(DX::Font font);
	void DrawText(const WCHAR *text, DX::Font font, FLOAT size, FLOAT x, FLOAT y, UINT32 color);

	void Clear();
	void BeginSpriteBatch();
	void EndSpriteBatch();
	void Render();
	HBITMAP Capture();
	void Screenshot(LPCWSTR fileName);
protected:
	int m_iWidth;
	int m_iHeight;

	// DirectX
	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr<ID3D11Device> m_d3dDevice;
	ComPtr<ID3D11DeviceContext> m_d3dContext;

	DXGI_FORMAT m_renderTargetFormat;
	CD3D11_TEXTURE2D_DESC m_renderTargetDesc;
	ComPtr<ID3D11Texture2D> m_renderTarget;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	DXGI_FORMAT m_depthStencilFormat;
	CD3D11_TEXTURE2D_DESC m_depthStencilDesc;
	ComPtr<ID3D11Texture2D> m_depthStencil;
	CD3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	CD3D11_VIEWPORT m_viewport;

	// DirectX Tool Kit
	std::unique_ptr<CommonStates> m_commonStates;
	std::unique_ptr<SpriteBatch> m_spriteBatch;

	// FW1 DX::Font Wrapper
	ComPtr<IFW1Factory> m_fw1FontFactory;

	void CreateDevice();
	void CreateResources();
};
