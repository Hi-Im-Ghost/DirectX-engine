#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

// sprawdzanie wyj¹tków grafiki/wyrzucanie makr
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
	//Tworzenie buforów, ³añcuchów i kontekstu dla renderowania 
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
#ifndef NDEBUG //Jesli w trybie debugowania to wywo³ujemy menedazer aby uzyskac wszystkie informacje
	infoManager.Set();
#endif//Jeœli obecna klatka da kod b³êdu, który oznacza usuniecie urz¹dzenia i jest on specjalny
	if (FAILED(hr = gSwap->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			//Wywo³aj usuniecie urz¹dzenia
			throw GFX_DEVICE_REMOVED_EXCEPT(gDevice->GetDeviceRemovedReason());
		}
		else
		{	//Jeœli nie to tworzymy zwyk³y wyj¹tek graficzny
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
	//Struktura wierzcho³ków
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
	//Wype³nienie w³aœciwoœci buffora
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
	//Ustawienie buforów dla wierzcho³ków
	gContext->IASetVertexBuffers(0u,1u,&gVertexBuffer,&stride,&offset);


	//Tworzenie vertex shadera
	wrl::ComPtr<ID3D11VertexShader> gVertexShader;
	//Wczytywanie pliku do bloba czyli pliku danych
	wrl::ComPtr<ID3DBlob> gBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &gBlob));
	//Urzadzenie tworzy vertex shader
	GFX_THROW_INFO(gDevice->CreateVertexShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gVertexShader));
	

	//Powi¹zanie vertex shadera z potokiem
	gContext->VSSetShader(gVertexShader.Get(), nullptr, 0u);


	//Tworzenie pixel shadera
	wrl::ComPtr<ID3D11PixelShader> gPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &gBlob));
	GFX_THROW_INFO(gDevice->CreatePixelShader(gBlob->GetBufferPointer(), gBlob->GetBufferSize(), nullptr, &gPixelShader));

	// Powi¹zanie pixel shadera z potokiem
	gContext->PSSetShader(gPixelShader.Get(), nullptr, 0u);


	GFX_THROW_INFO_ONLY(gContext->Draw((UINT)std::size(vertices), 0u));
}

// Wyj¹tki grafiki d3d
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// po³¹cz wszystkie wiadomoœci informacyjne z nowymi liniami w jeden ci¹g
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// usuñ ostatni¹ now¹ liniê, jeœli istnieje
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
	// po³¹cz wszystkie wiadomoœci informacyjne z nowymi liniami w jeden ci¹g
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// usuñ ostatni¹ now¹ liniê, jeœli istnieje
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