#pragma once
#include "ChiliWin.h"
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>
#include <string>

class DxgInfo
{
public:
	DxgInfo();
	~DxgInfo() = default;
	DxgInfo(const DxgInfo&) = delete;
	DxgInfo& operator=(const DxgInfo&) = delete;
	//Metoda do ustawiania kolejnych wiadomosci
	void Set() noexcept;
	//Metoda do pobierania kolejnych wiadomosci 
	std::vector<std::string> GetMessages() const;
private:
	//Indeks ostatniej wiadomoœci w kolejce
	unsigned long long next = 0u;
	//Interfejs z informacjami o DXGI
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> dDxgiInfoQueue;
};

