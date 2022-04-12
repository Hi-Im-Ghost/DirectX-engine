#pragma once
#include "ChiliWin.h"
#include <sstream>
#include "WindowsMessageManager.h"

//Klasa reprezentuj¹ca okno
class Window
{
private:
	//singleton
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
	Window(int width, int height, const char* name) noexcept;
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

