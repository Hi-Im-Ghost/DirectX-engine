#pragma once
#include "ChiliWin.h"
#include "Exception.h"
#include <sstream>
#include "WindowsMessageManager.h"

//Klasa reprezentuj¹ca okno
class Window
{
public:
	class Exceptions : public Exception
	{
	public:
		Exceptions(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		//T³umaczenie kodu b³edu
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
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
	//Konstruktor tworzacy okno
	Window(int width, int height, const char* name);
	//Destruktor niszczacy okno
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	//Metoda do konfigurowania wiadomosci
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	//Metoda do obslugi wiadomosci
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	//Obs³uga wiadomoœci
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
};

// Makro dla wyjatków
#define CHWND_EXCEPT( hr ) Window::Exceptions( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::Exceptions(__LINE__,__FILE__,GetLastError())
