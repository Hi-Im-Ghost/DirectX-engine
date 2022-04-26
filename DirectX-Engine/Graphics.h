#pragma once
#include "ChiliWin.h"
#include <d3d11.h>

class Graphics
{
private:
	//WskaŸniki do poszczególnych opcji dla d3d11
	ID3D11Device* gDevice = nullptr;
	IDXGISwapChain* gSwap = nullptr;
	ID3D11DeviceContext* gContext = nullptr;
	ID3D11RenderTargetView* gTarget = nullptr;
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	//Bufor tylni 
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
};

