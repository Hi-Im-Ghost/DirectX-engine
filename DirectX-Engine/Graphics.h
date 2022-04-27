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
	
#ifndef NDEBUG //Zamiesci nazwe menada¿era ale tylko w tybie debugowania
	DxgInfo infoManager;
#endif
	//WskaŸniki do poszczególnych opcji dla d3d11
	Microsoft::WRL::ComPtr<ID3D11Device> gDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> gSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> gContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> gTarget;
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	//Bufor tylni 
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;

	//Klasy wyj¹tków graficznych
	//Wyj¹tek grafiki bazowej
	class Exceptions : public Exception
	{
		using Exception::Exception;
	};
	//Wyj¹tek wyniku
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		//Uzyskaj ci¹g b³êdu
		std::string GetErrorString() const noexcept;
		//Uzyskaj opis b³êdu
		std::string GetErrorDescription() const noexcept;
		//Uzyskaj ciag dodatkowych informacji bledu
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		//Ciag dla dodatkowych informacji
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

