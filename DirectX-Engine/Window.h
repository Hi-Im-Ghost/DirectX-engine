#pragma once
#include "ChiliWin.h"
#include "Exception.h"
#include <sstream>
#include "WindowsMessageManager.h"
#include "Keyboard.h"
#include "Mouse.h"

//Klasa reprezentująca okno
class Window
{
private:
	int width;
	int height;
	HWND hWnd;

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
	class Exceptions : public Exception
	{
	public:
		Exceptions(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		//Tłumaczenie kodu błedu
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
	//Konstruktor tworzacy okno
	Window(int width, int height, const char* name);
	//Destruktor niszczacy okno
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	//Metoda do zmiany tytulu okna
	void SetTitle(const std::string& title);

	Keyboard kbd;
	Mouse mouse;
};

// Makro dla wyjatków
#define CHWND_EXCEPT( hr ) Window::Exceptions( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::Exceptions(__LINE__,__FILE__,GetLastError())
