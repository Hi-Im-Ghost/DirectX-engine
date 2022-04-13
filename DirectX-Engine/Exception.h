#pragma once
#include <exception>
#include <string>
//Klasa wyj¹tków
class Exception : public std::exception
{
public:
	Exception(int line, const char* file) noexcept;
	const char* what() const noexcept override;
	//Pobierz typ wyjatku
	virtual const char* GetType() const noexcept;
	//Pobierz numer wiersza
	int GetLine() const noexcept;
	//Pobierz nazwe pliku
	const std::string& GetFile() const noexcept;
	//Pobierz ciag znakow
	std::string GetOriginString() const noexcept;
private:
	//Numer wiersza wyjatku
	int line;
	//Plik z ktorego pochodzi wyjatek
	std::string file;
protected:
	//Buffor pozwoli przechowywaæ wyj¹tek po zakoñczeniu programu
	mutable std::string buffer;

};

