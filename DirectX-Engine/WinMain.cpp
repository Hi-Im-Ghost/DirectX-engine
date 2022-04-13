#include "Window.h"
#include <sstream>

int CALLBACK WinMain( 
	HINSTANCE hInstance, //wskaŸniki do struktur, które przechowuj¹ informacje o naszym programie
	HINSTANCE hPrevInstance, //w którym s¹ ³adowane do pamiêci. Drugi zawsze bêdzie NULL. 
	LPSTR lpCmdLine, //Linia poleceñ jako pojedynczy ci¹g znaków
	int nCmdShow) //Info w jaki sposób okno powinno byæ pokazane
{
	try {
		Window wind(800, 480, "DenginX");

		//Struktura wiadomoœci
		MSG msg;
		//
		BOOL gResult;
		//Pêtla g³ówna
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			//Przetwarzanie wiadomosci
			TranslateMessage(&msg);
			//Przekazanie wiadomoœci do okna
			DispatchMessage(&msg);
		}
		static int i = 0;
		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			// test code
			while (!wind.mouse.IsEmpty())
			{
				const auto e = wind.mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::Leave:
					wind.SetTitle("Gone!");
					break;
				case Mouse::Event::Type::WheelUp:
					i++;
					{
						std::ostringstream oss;
						oss << "Up: " << i;
						wind.SetTitle(oss.str());
					}
					break;
				case Mouse::Event::Type::WheelDown:
					i--;
					{
						std::ostringstream oss;
						oss << "Down: " << i;
						wind.SetTitle(oss.str());
					}
					break;
				case Mouse::Event::Type::Move:
				{
					std::ostringstream oss;
					oss << "Mouse moved to (" << e.GetPosX() << "," << e.GetPosY() << ")";
					wind.SetTitle(oss.str());
				}
				break;
				}
			}
		}
		if (gResult == -1)
		{
			return -1;
		}
		else
		{
			return msg.wParam;
		}
	}
	catch (const Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "WinMain Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}