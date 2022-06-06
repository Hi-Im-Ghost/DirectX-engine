#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

//Klasa obsługująca światło
class PointLight
{
public:
	//Konstruktor światła
	PointLight(Graphics& gfx, float radius = 0.5f);
	//Metoda do tworzenia okna dla obsługi światła
	void SpawnControlWindow() noexcept;
	//Metoda do restowania ustawień
	void Reset() noexcept;
	//Metoda do rysowania
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	//Metoda do ustawiania
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
private:
	//Struktura, która przechowuje ustawienia światła
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};