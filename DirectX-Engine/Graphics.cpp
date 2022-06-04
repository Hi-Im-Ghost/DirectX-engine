#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"

#include "ImGui/imgui_impl_dx11.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")




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
	sd.OutputWindow = hWnd; //do przetestowania wyrzucania wyjatkow piszemy (HWND)liczby
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG //Jesli w trybie debugowania to umieszczamy inny przelacznik do kontrolowania flagi do tworzenia urzadzenia
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Do sprawdzania wyników funkcji d3d
	HRESULT hr;
	//Tworzenie buforów, łańcuchów i kontekstu dla renderowania 
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0, //wyrzucanie wyjatku, zmiana urzadzenia np jako debugowane D3D11_CREATE_DEVICE_DEBUG
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&gSwap,
		&gDevice,
		nullptr,
		&gContext
	));
	// Uzyskanie dostepu do bufora tylnego
	// Szablon na typ interfejsu
	wrl::ComPtr<ID3D11Resource> gBackBuffer;
	// Przeciazenie operatora za pomoca wskaznika do komunikacji
	GFX_THROW_INFO(gSwap->GetBuffer(0, __uuidof(ID3D11Resource), &gBackBuffer));
	GFX_THROW_INFO(gDevice->CreateRenderTargetView(gBackBuffer.Get(), nullptr, &gTarget));

	// Utwórz stan Z-Buffera
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	wrl::ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(gDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	// Przypisz stan
	gContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	// Utwórz teksturę szablonu głębokości (stencil buffer)
	wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = 800u;
	descDepth.Height = 600u;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(gDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	// Tworzenie widoku z utworzonej tekstury
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(gDevice->CreateDepthStencilView(pDepthStencil.Get(), &descDSV, &gDSV));

	// Przypisz widok do OM
	gContext->OMSetRenderTargets(1u, gTarget.GetAddressOf(), gDSV.Get());

	// Konfiguracja viewportu
	D3D11_VIEWPORT vp;
	vp.Width = 800.0f;
	vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	gContext->RSSetViewports(1u, &vp);


	/* ImGUI */
	ImGui_ImplDX11_Init(gDevice.Get(), gContext.Get());
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG //Jesli w trybie debugowania to wywołujemy menedazer aby uzyskac wszystkie informacje
	infoManager.Set();
#endif//Jeśli obecna klatka da kod błędu, który oznacza usuniecie urządzenia i jest on specjalny
	if (FAILED(hr = gSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//Wywołaj usuniecie urządzenia
			throw GFX_DEVICE_REMOVED_EXCEPT(gDevice->GetDeviceRemovedReason());
		}
		else
		{	//Jeśli nie to tworzymy zwykły wyjątek graficzny
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red,green,blue,1.0f };
	gContext->ClearRenderTargetView(gTarget.Get(), color);
	gContext->ClearDepthStencilView(gDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	//Rysuj
	GFX_THROW_INFO_ONLY(gContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return projection;
}

// Wyjątki grafiki d3d
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// połącz wszystkie wiadomości informacyjne z nowymi liniami w jeden ciąg
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// usuń ostatnią nową linię, jeśli istnieje
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return info;
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
{
	// połącz wszystkie wiadomości informacyjne z nowymi liniami w jeden ciąg
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// usuń ostatnią nową linię, jeśli istnieje
	if (!info.empty())
	{
		info.pop_back();
	}
}


const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return info;
}