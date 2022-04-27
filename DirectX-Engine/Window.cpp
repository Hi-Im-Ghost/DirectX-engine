#include "Window.h"
#include <sstream>
#include "resource.h"

//
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept:
	hInst(GetModuleHandle(nullptr)) //Uzyskiwanie instancji do zapisywania danych
{
	WNDCLASSEX winclass = { 0 }; //struktura konfiguracyjna 
	winclass.cbSize = sizeof(winclass); //ustawienie rozmiaru struktury 
	winclass.style = CS_OWNDC; //ustawienie stylu
	winclass.lpfnWndProc = HandleMsgSetup; //wskaŸnik do procedury okna
	winclass.cbClsExtra = 0; //iloœæ dodatkowych bajtów w strukturze po stronie API
	winclass.cbWndExtra = 0; //iloœæ dodatkowych bajtów dla ka¿dego okna, które zosta³o stworzone przez t¹ klase
	winclass.hInstance = GetInstance(); //dojœcie do instancji aplikacji
	winclass.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); //ikona 
	winclass.hCursor = nullptr; //kursor
	winclass.hbrBackground = nullptr; //t³o
	winclass.lpszMenuName = nullptr; //nazwa menu
	winclass.lpszClassName = GetName(); //nazwa klasy
	winclass.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0)); ; //ikona
	RegisterClassEx(&winclass); //rejestracja klasy. Ex oznacza nowsz¹ wersje
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}


// 
Window::Window(int width, int height, const char* name):
	width(width),
	height(height)
{
	// rozmiar okna
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	//Dostosowanie by rozmiar okna by³ dla ca³oœci po¿¹danego regionu klienta
	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE)==0)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Instancja okna
	hWnd = CreateWindow(
		WindowClass::GetName(), //nazwa klasy
		name, //nazwa okna
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wr.right - wr.left, //po³o¿enie okna
		wr.bottom - wr.top, //rozdzielczoœæ
		nullptr, //wskaŸnik na rodzica okna
		nullptr, //wskaŸnik do menu
		WindowClass::GetInstance(), //instancja
		this //niestandardowy
	);
	//Sprawdzanie bledow
	if (hWnd == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}
	//Wyœwietl okno
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	//Tworzenie obrazu grafiki d3d
	gD3g = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& title)
{

	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw CHWND_LAST_EXCEPT();
	}
}


std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	// Gdy kolejka zawiera powiadomienia to je usuñ i wyœlij
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// Sprawdzanie czy wyszlismy 
		if (msg.message == WM_QUIT)
		{
			// opcjonalnie zwróc sygnalizacje do wyjscia
			return (int)msg.wParam;
		}

		// Publikowanie powiadomien
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Nic nie zwraca gdy nie zamykamy aplikacji
	return {};
}

Graphics& Window::D3g()
{
	//Wyj¹tek gdy spróbujemy uzyskaæ grafikê ale wskaŸnik nie zosta³ jeszcze ustawiony
	if (!gD3g)
	{
		throw CHWND_NOGFX_EXCEPT();
	}
	return *gD3g;
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Sprawdzenie czy typ wiadomosci jest rowny tworzonemu bez uzytkownika
	if (msg == WM_NCCREATE)
	{
		// wyodrêbnij wskaŸnik do klasy okna z danych tworzenia
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		// statyczne rzutowanie tworzenia okna 
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// ustaw dane u¿ytkownika zarz¹dzane przez WinAPI do przechowywania wskaŸnika do klasy okna
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// ustaw procedurê obs³ugi wiadomoœci na normaln¹ (nie-instalacyjn¹) procedurê obs³ugi po zakoñczeniu instalacji
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// przeœlij wiadomoœæ do programu obs³ugi klasy okna
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	//Jesli nie to zrob domyslnie
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// pobierz wskaznik do klasy okna
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// przeœlij wiadomoœæ do programu obs³ugi klasy okna
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

//Procedura dla okna
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	//Akcja po nacisnieciu X okna
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	//wyczyœæ stan klawiszy, gdy okno traci fokus, aby zapobiec "zablokowaniu" danych wejœciowych
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	//KLAWIATURA
	//Akcja po naciscieniu klawisza
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) //autorepeat
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	//MYSZKA
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// w oknie przenos i przechwycaj
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// jesli nie w oknie to utrzymuj przechwytwanie jesli trzymamy przycisk
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// jesli przycisk puscimy to zwolnij przechtytywanie 
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	}

	//Zwracamy procedure okna
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Wyj¹tki okna
std::string Window::Exceptions::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	// Przydziel pamiêc dla ³añcucha b³êdów i wska¿ na niego nasz wskaŸnik
	const DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	// Zwrócona d³ugoœæ ci¹gu 0 oznacza awariê
	if (nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	// skopiuj ci¹g b³êdu z bufora przydzielonego do systemu Windows do std::string
	std::string errorString = pMsgBuf;
	// zwolnij bufor systemu
	LocalFree(pMsgBuf);
	return errorString;
}


Window::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	Exception(line, file),
	hr(hr)
{}

const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	buffer = oss.str();
	return buffer.c_str();
}

const char* Window::HrException::GetType() const noexcept
{
	return "Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::HrException::GetErrorDescription() const noexcept
{
	return Exceptions::TranslateErrorCode(hr);
}


const char* Window::NoGfxException::GetType() const noexcept
{
	return "Window Exception [No Graphics]";
}