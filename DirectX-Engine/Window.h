#pragma once
#include <sstream>
#include <optional>
#include <memory>
#include "ChiliWin.h"
#include "Exception.h"
#include "Message.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"


//Klasa reprezentująca okno
class Window
{
private:
	int width;
	int height;
	HWND hWnd;
	//Unikalny skaźnik dla grafiki d3d
	std::unique_ptr<Graphics> gD3g;

	//Metoda do konfigurowania wiadomosci
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	//Metoda do obslugi wiadomosci
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	//Obsługa wiadomości
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	class WindowClass
	{
	public:
		//Metoda do pobierania nazwy klasy
		static const char* GetName() noexcept;
		//Metoda do pobierania instancji 
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "DirectX-Engine";
		//Instancja klasy
		static WindowClass wndClass;
		HINSTANCE hInst;
	};

public:
	//Klasa bazowa wyjątków
	class Exceptions : public Exception
	{
		using Exception::Exception;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hr;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const char* GetType() const noexcept override;
	};
	//Konstruktor tworzacy okno
	Window(int width, int height, const char* name);
	//Destruktor niszczacy okno
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	//Metoda do zmiany tytulu okna
	void SetTitle(const std::string& title);
	static std::optional<int> ProcessMessages();
	//Uzyskiwanie dostępu do osadzonej grafiki
	Graphics& D3g();

	Keyboard kbd;
	Mouse mouse;
};

// Makro dla wyjatków
#define CHWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define CHWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )