#include "App.h"
#include "Box.h"
#include <memory>
#include <algorithm>
#include "DMath.h"
#include "Melon.h"
#include "Pyramid.h"

App::App() : win(800, 600, "DenginX")
{
	class MakeObject {
	public:
		MakeObject(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist(rng))
			{
			case 0:
				return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 1:
				return std::make_unique<Box>(
					gfx, rng, adist, ddist,
					odist, rdist, bdist
					);
			case 2:
				return std::make_unique<Melon>(
					gfx, rng, adist, ddist,
					odist, rdist, longdist, latdist
					);
			default:
				assert(false && "bad drawable type in factory");
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,2 };
	};

	MakeObject make(win.D3g());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, make);
	win.D3g().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
	const auto dt = timer.Mark();
	win.D3g().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& d : drawables)
	{
		d->Update(dt);
		d->Draw(win.D3g());
	}
	win.D3g().EndFrame();
}