#include "Graphics.h"
#pragma comment(lib,"d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
	//Struktura deskryptorów
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//Tworzenie buforów, ³añcuchów i kontekstu dla renderowania 
	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&gSwap,
		&gDevice,
		nullptr,
		&gContext
	);
	// Uzyskanie dostepu do bufora tylnego
	ID3D11Resource* gBackBuffer = nullptr;
	gSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&gBackBuffer));
	gDevice->CreateRenderTargetView(
		gBackBuffer,
		nullptr,
		&gTarget
	);
	gBackBuffer->Release();
}

Graphics::~Graphics()
{
	//Sprawdzanie czy stworzyliœmy urz¹dzenie
	if (gDevice != nullptr)
	{
		//Jeœli stworzyliœmy to zwolnij ...
		if (gTarget != nullptr)
		{
			gTarget->Release();
		}
		if (gContext != nullptr)
		{
			gContext->Release();
		}
		if (gSwap != nullptr)
		{
			gSwap->Release();
		}
		if (gDevice != nullptr)
		{
			gDevice->Release();
		}
	}
}

void Graphics::EndFrame()
{
	//Ustawienie interwa³u prezentacji czyli ile klatek ma sie wygenerowaæ
	gSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	gContext->ClearRenderTargetView(gTarget, color);
}
