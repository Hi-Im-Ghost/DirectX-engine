#pragma once
#include <Windows.h>
#include <unordered_map>
//Klasa do zarz¹dzania wiadomoœciami 
class WindowsMessageManager
{
public:
	WindowsMessageManager();
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;
private:
	//Mapowanie id wiadomosci Windowsa na nazwe wiadomosci
	std::unordered_map<DWORD, std::string> map;
};

