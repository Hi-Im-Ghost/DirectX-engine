#include "DxgInfo.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

DxgInfo::DxgInfo()
{
	// Definiowanie sygnaturty funkcji dxdgi
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// wczytywanie biblioteki dll zawierajaca dxgi
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	// pobieranie adresu dxgi w dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
		);
	if (DxgiGetDebugInterface == nullptr)
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &dDxgiInfoQueue));
}

void DxgInfo::Set() noexcept
{
	// ustawianie indeksu tak aby wszystkie dostala funkcja getmessages
	// otrzyma tylko bledy wygenerowane po tym wywolaniu
	next = dDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgInfo::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = dDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		// pobieranie rozmiaru wiadomoœci w bajtach
		GFX_THROW_NOINFO(dDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		// allocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		// pobieranie wiadomosci i wstawainie info do wektora
		GFX_THROW_NOINFO(dDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}