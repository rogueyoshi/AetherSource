#pragma once

#include <vector>
#include <wrl/client.h>
#include <d3d11_1.h>

#include "CommonStates.h"
#include "SpriteBatch.h"

using namespace Microsoft::WRL;
using namespace DirectX;

// "I'm not a wrapper."
class CDirectXWrapper
{
public:
	CDirectXWrapper();
	~CDirectXWrapper();

	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	void SetResolution(int iWidth, int iHeight);

	int LoadImage(const wchar_t *filePath);
	void DeleteImage(int index);

	void BeginSpriteBatch();
	void EndSpriteBatch();
	void DrawSprite(int index, float xPosition, float yPosition);

	void Clear();
	void Flush();
	void Screenshot();
	HBITMAP Capture();
protected:
	void CreateDevice();
	void CreateResources();

	int m_iWidth;
	int m_iHeight;

	// DirectX
	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr<ID3D11Device> m_d3dDevice;
	ComPtr<ID3D11DeviceContext> m_d3dContext;
	DXGI_FORMAT textureFormat;
	CD3D11_TEXTURE2D_DESC m_textureDesc;
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	DXGI_FORMAT depthStencilFormat;
	CD3D11_TEXTURE2D_DESC depthStencilDesc;
	ComPtr<ID3D11Texture2D> depthStencil;
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	CD3D11_VIEWPORT m_viewport;

	// DirectXTK
	std::unique_ptr<CommonStates> m_commonStates;
	std::unique_ptr<SpriteBatch> m_spriteBatch;
	std::vector<ComPtr<ID3D11ShaderResourceView>> m_sprites;
};
