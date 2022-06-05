#include "Camera.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto pos = dx::XMVector3Transform(
		dx::XMVectorSet(0.0f, 0.0f, -r, 0.0f),
		dx::XMMatrixRotationRollPitchYaw(phi, -theta, 0.0f)
	);
	return dx::XMMatrixLookAtLH(
		pos, dx::XMVectorZero(),
		dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
	) * dx::XMMatrixRotationRollPitchYaw(
		pitch, -yaw, roll
	);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Pozycja");
		ImGui::SliderFloat("R (przyblij-oddal)", &r, 0.0f, 80.0f, "%.1f");
		ImGui::SliderAngle("THETA (Obrot wokol rownika)", &theta, -180.0f, 180.0f);
		ImGui::SliderAngle("PHI (Obrot polnoc-poludnie)", &phi, -89.0f, 89.0f);
		ImGui::Text("Orientacja");
		ImGui::SliderAngle("Roll (obrot)", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch (gora-dol)", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw (lewo-prawo)", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}