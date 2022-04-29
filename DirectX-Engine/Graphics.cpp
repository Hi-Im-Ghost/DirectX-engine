#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

// sprawdzanie wyjątków grafiki/wyrzucanie makr
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG //Inne zachowanie w zaleznosci czy jestesmy w trybie debugowania czy nie
//Jesli w trybie debugowania to uzywamy menadzera informacji aby dodac info do naszego wyjatku
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

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
}

void Graphics::DrawTriangle()
{
	namespace wrl = Microsoft::WRL;
	HRESULT hr;
	//Struktura wierzchołków
	struct Vertex
	{
		float x;
		float y;
	};

	const Vertex vertices[] =
	{
		{0.0f,0.5f},
		{0.5f,-0.5f},
		{-0.5f,-0.5f},
	};

	wrl::ComPtr<ID3D11Buffer> gVertexBuffer;
	D3D11_BUFFER_DESC bufferDesc = {};
	//Wypełnienie właściwości buffora
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.MiscFlags = 0u;
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA subresData = {};
	//Ustawienie pamieci systemowej
	subresData.pSysMem = vertices;
	//Tworzenie bufora
	GFX_THROW_INFO(gDevice->CreateBuffer(&bufferDesc,&subresData,&gVertexBuffer));


	//Kroki
	const UINT stride = sizeof(Vertex);
	//Przesuniecia
	const UINT offset = 0u;
	//Ustawienie buforów dla wierzchołków
	gContext->IASetVertexBuffers(0u,1u,gVertexBuffer.GetAddressOf(), &stride, &offset);


	//Tworzenie pixel shadera
	wrl::ComPtr<ID3D11PixelShader> gPixelShader;
	//Wczytywanie pliku do bloba czyli pliku danych
	wrl::ComPtr<ID3DBlob> gBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &gBlob));
	GFX_THROW_INFO(gDevice->CreatePixelShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gPixelShader));

	// Powiązanie pixel shadera z potokiem
	gContext->PSSetShader(gPixelShader.Get(), nullptr, 0u);


	//Tworzenie vertex shadera
	wrl::ComPtr<ID3D11VertexShader> gVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &gBlob));
	//Urzadzenie tworzy vertex shader
	GFX_THROW_INFO(gDevice->CreateVertexShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gVertexShader));
	
	//Powiązanie vertex shadera z potokiem
	gContext->VSSetShader(gVertexShader.Get(), nullptr, 0u);


	//układ danych wejściowych
	wrl::ComPtr<ID3D11InputLayout> gInputLayout;
	const D3D11_INPUT_ELEMENT_DESC inputElement[] =
	{
		{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
	};
	GFX_THROW_INFO(gDevice->CreateInputLayout(
		inputElement, (UINT)std::size(inputElement),
		gBlob->GetBufferPointer(),
		gBlob->GetBufferSize(),
		&gInputLayout
	));

	// Powiązanie vertex shadera
	gContext->IASetInputLayout(gInputLayout.Get());


	// Powiązanie renderowania
	gContext->OMSetRenderTargets(1u, gTarget.GetAddressOf(), nullptr);


	// Ustaw topologię pierwotną na listę trójkątów
	gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// skonfiguruj widok
	D3D11_VIEWPORT viewPort;
	viewPort.Width = 800;
	viewPort.Height = 600;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	gContext->RSSetViewports(1u, &viewPort);

	//Rysuj
	GFX_THROW_INFO_ONLY(gContext->Draw((UINT)std::size(vertices), 0u));
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