#pragma once
#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist);
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	// zmienne pozycjonujące
	float r;
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float theta;
	float phi;
	float chi;

	// prędkość (delta/s)
	/* zmiana w przestrzeni wymiarowej kostki */
	float droll;
	float dpitch;
	float dyaw;
	/* zmiana w przestrzeni wymiarowej świata */
	float dtheta;
	float dphi;
	float dchi;
	// Przekształcenie modelu
	DirectX::XMFLOAT3X3 mt;
};