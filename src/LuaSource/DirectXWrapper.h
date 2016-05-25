#pragma once

#include <thread>
#include <vector>
#include <wrl/client.h>
#include <d3d11_1.h>

#include "CommonStates.h"
#include "SpriteBatch.h"
#include "Keyboard.h"
#include "GamePad.h"
#include "FW1FontWrapper.h"

using namespace Microsoft::WRL;
using namespace DirectX;

typedef ID3D11ShaderResourceView *Image;

// "I'm not a wrapper."
class CDirectXWrapper
{
public:
	CDirectXWrapper();
	~CDirectXWrapper();

	int GetWidth() { return m_iWidth; }
	int GetHeight() { return m_iHeight; }
	void SetResolution(int iWidth, int iHeight);

	Keyboard::State GetKeyboardState() { return m_keyboard->GetState(); }

	Image LoadImage(const wchar_t *filePath);
	void DeleteImage(Image image);

	void BeginSpriteBatch();
	void EndSpriteBatch();
	void DrawSprite(Image image, float xPosition, float yPosition);
	void DrawText(const WCHAR *text, LPCWSTR font, FLOAT size, FLOAT x, FLOAT y, UINT32 color);

	void Clear();
	void Render();
	void Screenshot();
	HBITMAP Capture();
protected:
	int m_iWidth;
	int m_iHeight;

	void CreateDevice();
	void CreateResources();

	// Windows
	static HHOOK m_hHook;
	std::thread *m_pHookThread;

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

	// DirectX Tool Kit
	std::unique_ptr<CommonStates> m_commonStates;
	std::unique_ptr<SpriteBatch> m_spriteBatch;
	std::unique_ptr<Keyboard> m_keyboard;
	std::unique_ptr<GamePad> m_gamePad;

	// FW1 Font Wrapper
	IFW1Factory *m_fw1FontFactory;
	IFW1FontWrapper *m_fw1FontWrapper;
};
