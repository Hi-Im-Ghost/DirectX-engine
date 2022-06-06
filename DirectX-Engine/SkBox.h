#pragma once
#include "TestObject.h"
//Klasa do tworzenia kostek z textura
class SkBox : public TestObject<SkBox>
{
public:
	SkBox(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
};