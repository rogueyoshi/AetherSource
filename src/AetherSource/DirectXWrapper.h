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

// "I'm not a wrapper."
class CDirectXWrapper
{
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

	std::vector<uint8_t> m_iBuffer;

	DXGI_FORMAT m_depthStencilFormat;
	CD3D11_TEXTURE2D_DESC m_depthStencilDesc;
	ComPtr<ID3D11Texture2D> m_depthStencil;
	CD3D11_DEPTH_STENCIL_VIEW_DESC m_depthStencilViewDesc;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	CD3D11_VIEWPORT m_viewport;

	// DirectX Tool Kit
	std::unique_ptr<CommonStates> m_commonStates;
	std::unique_ptr<SpriteBatch> m_spriteBatch;

	// FW1 IFW1FontWrapper *Wrapper
	ComPtr<IFW1Factory> m_fw1FontFactory;

	void CreateDevice();
	void CreateResources();
public:
	CDirectXWrapper();
	~CDirectXWrapper();

	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	void SetResolution(int iWidth, int iHeight);

	void ClearScreen(float redBlend, float greenBlend, float blueBlend, float alphaBlend);

	ID3D11ShaderResourceView *LoadTexture(const wchar_t *filePath);
	void ReleaseTexture(ID3D11ShaderResourceView *texture);
	void DrawSprite(ID3D11ShaderResourceView *texture, float xPosition, float yPosition, float redBlend, float greenBlend, float blueBlend, float alphaBlend);

	IFW1FontWrapper *LoadFont(LPCWSTR fontFamily);
	void ReleaseFont(IFW1FontWrapper *font);
	void DrawText(const WCHAR *text, IFW1FontWrapper *font, FLOAT size, FLOAT x, FLOAT y, UINT32 color);

	void BeginSpriteBatch();
	void EndSpriteBatch();
	void Render();
	HBITMAP Capture();
	void Screenshot(LPCWSTR fileName);
};
