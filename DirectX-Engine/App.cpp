#include "App.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Sheet.h"
#include "SkBox.h"
#include <memory>
#include <algorithm>
#include "DMath.h"
#include "Surface.h"
#include "TextureManager.h"

#include "ImGui/imgui.h"

TextureManager gdipm;
namespace dx = DirectX;

App::App() : win(800, 600, "DenginX")
{
	class MakeObject
	{
	public:
		MakeObject(Graphics& win)
			:
			win(win)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					win, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					win, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					win, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			case 3:
				return std::make_unique<Sheet>(
					win, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkBox>(
					win, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& win;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, MakeObject{ win.D3g() });

	win.D3g().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));

}

int App::Start()
{
	while (true)
	{
		// Przetwarzanie powiadomien
		if (const auto code = Window::ProcessMessages())
		{
			// Jeśli wychodzimy to zwracamy kod wyjścia
			return *code;
		}
		DoFrame();
	}
}

App::~App()
{}

void App::DoFrame()
{
	//Przy pomocy sin w czasie manipulujemy kolorem
	/*const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	win.D3g().ClearBuffer(c, 1.0f, c);
	win.D3g().DrawCube(
		3.0f,
		3.0f,
		6.0f,

		0.0f,
		timer.Peek(),
		timer.Peek()
	);
	win.D3g().DrawCube(
		win.mouse.GetPosX() / 400.0f - 1.0f,
		-win.mouse.GetPosY() / 300.0f + 1.0f,
		4.0f,

		45.0f,
		45.0f,
		0.0f
	);
	win.D3g().DoFrame();*/
	const auto dt = timer.Mark() * speed_factor;
	win.D3g().BeginFrame(0.07f, 0.0f, 0.12f);
	win.D3g().SetCamera(cam.GetMatrix());
	for (auto& d : drawables)
	{
		d->Update(win.kbd.KeyIsPressed(VK_SPACE) ? 0.0f : dt);
		d->Draw(win.D3g());
	}

	// Okno imgui to kotnroli predkosci poruszania sie obiektow
	if (ImGui::Begin("Symulacja Predkosci"))
	{
		ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Text("Status: %s", win.kbd.KeyIsPressed(VK_SPACE) ? "PAUSED" : "RUNNING (wcisnij spacje by zatrzymac)");
	}
	ImGui::End();

	// okno imgui do sterowania kamera
	cam.SpawnControlWindow();

	win.D3g().EndFrame();
}