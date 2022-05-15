#pragma once
#include <queue>
#include <bitset>

//Klasa obsługi klawiatury
class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
		Event() noexcept
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	private:
		Type type;
		unsigned char code;
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// Akcje klawiszy
	// Klawisz wcisniety
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	// Odczyt klawisza
	Event ReadKey() noexcept;
	// Sprawdzenie czy klawisz jest w kolejce zdarzeń
	bool KeyIsEmpty() const noexcept;
	// Opróżnianie kolejki zdarzeń klawiszy
	void FlushKey() noexcept;
	// Akcje Char uzywane do wprowadzania strumienia znakow 
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	// autorepeat control
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	//Wywoływane gdy otrzymamy określone komunikaty systemu Windows
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	//Bufor przecinania usuwa elementy z kolejki jesli ktores przekrocza rozmiar zadeklarowany
	static void TrimBuffer(std::queue<T>&buffer) noexcept;
	//Maksymalna liczba kodów klawiszy
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	bool autorepeatEnabled = false;
	//Pakuje 256 bitów w pojedyncze bity i pozwala na dostęp do nich za pomocą indeksu
	std::bitset<nKeys> keystates;
	//Kolejka dla klawiszy
	std::queue<Event> keybuffer;
	//Kolejka dla znaków
	std::queue<char> charbuffer;
};

