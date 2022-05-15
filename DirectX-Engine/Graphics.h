#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include "ChiliWin.h"
#include "Exception.h"
#include "DxgInfo.h"

class Graphics
{
private:
	
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
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	//Bufor tylni 
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	//Metoda do rysowania trójkątów
	void DrawCube(float x, float y, float z, float rotationX, float rotationY, float rotationZ);


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

