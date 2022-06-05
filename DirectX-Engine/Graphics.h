#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "ChiliWin.h"
#include "Exception.h"
#include "DxgInfo.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class Graphics
{
	friend class Bindable;

public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	// Bufor tylni
	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	//Metoda do tworzenia kamery
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	//Metoda do pobierania pozycji kamery
	DirectX::XMMATRIX GetCamera() const noexcept;
	//Metoda do wlaczania imgui
	void EnableImgui() noexcept;
	//Metoda do wlaczania imgui
	void DisableImgui() noexcept;
	//Metoda do sprawdzania czy jest wlaczone imgui
	bool IsImguiEnabled() const noexcept;
	//Metoda do rysowania trójkątów
	//void DrawCube(float x, float y, float z, float rotationX, float rotationY, float rotationZ);

private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	//Zmienna do sprawdzania czy jest wlaczone okno imgui
	bool imguiEnabled = true;
#ifndef NDEBUG //Zamiesci nazwe menadażera ale tylko w tybie debugowania
	DxgInfo infoManager;
#endif

	//Wskaźniki com do poszczególnych opcji dla d3d11
	//Używany do przydzielania opcji
	Microsoft::WRL::ComPtr<ID3D11Device> gDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> gSwap;
	//Używany do konfigurowania potoku podczas wydawania poleceń renderowania
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> gContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> gDSV;

public:
	//Klasy wyjątków graficznych
	//Wyjątek grafiki bazowej
	class Exceptions : public Exception
	{
		using Exception::Exception;
	};
	//Wyjątek wyniku
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		//Uzyskaj ciąg błędu
		std::string GetErrorString() const noexcept;
		//Uzyskaj opis błędu
		std::string GetErrorDescription() const noexcept;
		//Uzyskaj ciag dodatkowych informacji bledu
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		//Ciag dla dodatkowych informacji
		std::string info;
	};
	//Wyjatek tylko z informacjami o bledzie 
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
};

