#pragma once
#include "Graphics.h"

//Klasa do zarzadzania kamera
class Camera
{
public:
	//Metoda do pobierania pozycji
	DirectX::XMMATRIX GetMatrix() const noexcept;
	//Metoda do tworzenia gui dla ustawien kamery
	void SpawnControlWindow() noexcept;
	//Metoda do restowania kamery
	void Reset() noexcept;
private:
	//Pozycja kamery na Z
	float r = 20.0f;
	//Pozycja kamery wokol rownika
	float theta = 0.0f;
	//Pozycja kamery polnoc-poludnie
	float phi = 0.0f;
	//Rotacja
	float pitch = 0.0f;
	//Gora-dol
	float yaw = 0.0f;
	//Lewo-prawo
	float roll = 0.0f;
};